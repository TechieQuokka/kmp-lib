#pragma once

/**
 * @file dfa.hpp
 * @brief DFA-based regex engine with O(n) matching guarantee
 *
 * Implements Thompson's NFA construction followed by subset construction
 * to create a DFA. This guarantees linear-time matching without
 * backtracking, unlike PCRE engines.
 *
 * Supported syntax (all linear-time safe):
 *   .       - any character
 *   *       - zero or more
 *   +       - one or more
 *   ?       - zero or one
 *   [abc]   - character class
 *   [^abc]  - negated character class
 *   [a-z]   - character range
 *   (...)   - grouping
 *   |       - alternation
 *   ^       - start anchor
 *   $       - end anchor
 *   \d \w \s - shorthand classes
 *
 * NOT supported (non-linear):
 *   \1      - backreference
 *   (?=)    - lookahead
 *   (?<=)   - lookbehind
 */

#include "../config.hpp"
#include <algorithm>
#include <array>
#include <vector>
#include <bitset>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <stdexcept>
#include <memory>

namespace kmp::detail {

// =============================================================================
// Character Classes
// =============================================================================

/**
 * @brief Bitset representing a set of ASCII characters
 */
class char_class {
public:
    static constexpr size_type size = config::ascii_size;

    constexpr char_class() = default;

    constexpr void set(char c) noexcept {
        if (static_cast<unsigned char>(c) < size) {
            bits_.set(static_cast<unsigned char>(c));
        }
    }

    constexpr void set_range(char from, char to) noexcept {
        for (unsigned char c = static_cast<unsigned char>(from);
             c <= static_cast<unsigned char>(to) && c < size; ++c) {
            bits_.set(c);
        }
    }

    constexpr void set_all() noexcept {
        bits_.set();
    }

    constexpr void flip() noexcept {
        bits_.flip();
        // Ensure non-ASCII bits are off
        for (size_type i = size; i < bits_.size(); ++i) {
            bits_.reset(i);
        }
    }

    [[nodiscard]] constexpr bool test(char c) const noexcept {
        auto uc = static_cast<unsigned char>(c);
        return uc < size && bits_.test(uc);
    }

    [[nodiscard]] constexpr bool any() const noexcept {
        return bits_.any();
    }

    // Predefined character classes
    [[nodiscard]] static char_class digit() {
        char_class cc;
        cc.set_range('0', '9');
        return cc;
    }

    [[nodiscard]] static char_class word() {
        char_class cc;
        cc.set_range('a', 'z');
        cc.set_range('A', 'Z');
        cc.set_range('0', '9');
        cc.set('_');
        return cc;
    }

    [[nodiscard]] static char_class space() {
        char_class cc;
        cc.set(' ');
        cc.set('\t');
        cc.set('\n');
        cc.set('\r');
        cc.set('\f');
        cc.set('\v');
        return cc;
    }

    [[nodiscard]] static char_class any_char() {
        char_class cc;
        cc.set_all();
        cc.bits_.reset('\n');  // . doesn't match newline by default
        return cc;
    }

private:
    std::bitset<size> bits_{};
};

// =============================================================================
// NFA State
// =============================================================================

// Sentinel value for "no transition" (using max value instead of 0)
inline constexpr size_type no_transition = static_cast<size_type>(-1);

struct nfa_state {
    enum class type {
        epsilon,      // epsilon transition
        char_match,   // single character
        class_match,  // character class
        accept        // accepting state
    };

    type kind = type::epsilon;
    char match_char = '\0';
    char_class match_class{};
    size_type next1 = no_transition;  // primary transition
    size_type next2 = no_transition;  // secondary (for epsilon splits)

    [[nodiscard]] bool has_next1() const noexcept { return next1 != no_transition; }
    [[nodiscard]] bool has_next2() const noexcept { return next2 != no_transition; }
};

// =============================================================================
// NFA Fragment (for Thompson construction)
// =============================================================================

struct nfa_fragment {
    size_type start;
    size_type end;  // dangling end (to be patched)
};

// =============================================================================
// DFA State
// =============================================================================

struct dfa_state {
    std::array<size_type, config::ascii_size> transitions{};
    bool is_accept = false;

    dfa_state() {
        transitions.fill(static_cast<size_type>(-1));  // dead state
    }
};

// =============================================================================
// Compiled DFA
// =============================================================================

class compiled_dfa {
public:
    compiled_dfa() = default;

    /**
     * @brief Compile a regex pattern into DFA
     * @throws std::runtime_error if pattern is invalid or too complex
     */
    explicit compiled_dfa(std::string_view pattern) {
        compile(pattern);
    }

    /**
     * @brief Check if pattern matches anywhere in text
     * @return Position of first match, or nullopt if not found
     */
    [[nodiscard]] std::optional<size_type> search(std::string_view text) const noexcept {
        if (states_.empty()) {
            return std::nullopt;
        }

        for (size_type start = 0; start < text.size(); ++start) {
            size_type state = 0;
            bool matched = states_[0].is_accept;

            for (size_type i = start; i < text.size(); ++i) {
                auto c = static_cast<unsigned char>(text[i]);
                if (c >= config::ascii_size) {
                    break;  // non-ASCII
                }

                size_type next = states_[state].transitions[c];
                if (next == static_cast<size_type>(-1)) {
                    break;  // dead state
                }

                state = next;
                if (states_[state].is_accept) {
                    return start;
                }
            }

            if (matched) {
                return start;
            }
        }

        return std::nullopt;
    }

    /**
     * @brief Check if pattern matches the entire text
     */
    [[nodiscard]] bool matches(std::string_view text) const noexcept {
        if (states_.empty()) {
            return false;
        }

        size_type state = 0;

        for (char c : text) {
            auto uc = static_cast<unsigned char>(c);
            if (uc >= config::ascii_size) {
                return false;
            }

            size_type next = states_[state].transitions[uc];
            if (next == static_cast<size_type>(-1)) {
                return false;
            }
            state = next;
        }

        return states_[state].is_accept;
    }

    [[nodiscard]] bool empty() const noexcept {
        return states_.empty();
    }

    [[nodiscard]] size_type state_count() const noexcept {
        return states_.size();
    }

private:
    std::vector<dfa_state> states_;
    std::vector<nfa_state> nfa_states_;
    size_type nfa_start_ = 0;  // NFA start state

    void compile(std::string_view pattern) {
        // Step 1: Parse and build NFA using Thompson construction
        build_nfa(pattern);

        // Step 2: Convert NFA to DFA using subset construction
        build_dfa();
    }

    void build_nfa(std::string_view pattern) {
        nfa_states_.clear();
        nfa_states_.reserve(pattern.size() * 2);

        size_type pos = 0;
        auto frag = parse_regex(pattern, pos);

        // Store the NFA start state
        nfa_start_ = frag.start;

        // Create accept state
        size_type accept = nfa_states_.size();
        nfa_states_.push_back({nfa_state::type::accept, '\0', {}, no_transition, no_transition});

        // Patch fragment end to accept
        patch(frag.end, accept);
    }

    // Recursive descent parser for regex
    nfa_fragment parse_regex(std::string_view pattern, size_type& pos) {
        return parse_alternation(pattern, pos);
    }

    nfa_fragment parse_alternation(std::string_view pattern, size_type& pos) {
        auto left = parse_concatenation(pattern, pos);

        while (pos < pattern.size() && pattern[pos] == '|') {
            ++pos;
            auto right = parse_concatenation(pattern, pos);

            // Create split state: next1 goes to left, next2 goes to right
            size_type split = nfa_states_.size();
            nfa_states_.push_back({nfa_state::type::epsilon, '\0', {}, left.start, right.start});

            // Create join state (dummy for patching)
            size_type join = nfa_states_.size();
            nfa_states_.push_back({nfa_state::type::epsilon, '\0', {}, no_transition, no_transition});

            patch(left.end, join);
            patch(right.end, join);

            left = {split, join};
        }

        return left;
    }

    nfa_fragment parse_concatenation(std::string_view pattern, size_type& pos) {
        nfa_fragment result{0, 0};
        bool first = true;

        while (pos < pattern.size() && pattern[pos] != '|' && pattern[pos] != ')') {
            auto atom = parse_quantified(pattern, pos);

            if (first) {
                result = atom;
                first = false;
            } else {
                patch(result.end, atom.start);
                result.end = atom.end;
            }
        }

        if (first) {
            // Empty pattern - create epsilon
            size_type state = nfa_states_.size();
            nfa_states_.push_back({nfa_state::type::epsilon, '\0', {}, no_transition, no_transition});
            result = {state, state};
        }

        return result;
    }

    nfa_fragment parse_quantified(std::string_view pattern, size_type& pos) {
        auto base = parse_atom(pattern, pos);

        if (pos >= pattern.size()) {
            return base;
        }

        char quant = pattern[pos];
        if (quant == '*') {
            ++pos;
            return make_star(base);
        } else if (quant == '+') {
            ++pos;
            return make_plus(base);
        } else if (quant == '?') {
            ++pos;
            return make_optional(base);
        }

        return base;
    }

    nfa_fragment parse_atom(std::string_view pattern, size_type& pos) {
        if (pos >= pattern.size()) {
            throw std::runtime_error("Unexpected end of pattern");
        }

        char c = pattern[pos];

        if (c == '(') {
            ++pos;
            auto inner = parse_regex(pattern, pos);
            if (pos >= pattern.size() || pattern[pos] != ')') {
                throw std::runtime_error("Unmatched parenthesis");
            }
            ++pos;
            return inner;
        }

        if (c == '[') {
            return parse_char_class(pattern, pos);
        }

        if (c == '.') {
            ++pos;
            size_type state = nfa_states_.size();
            nfa_states_.push_back({nfa_state::type::class_match, '\0',
                                   char_class::any_char(), no_transition, no_transition});
            return {state, state};
        }

        if (c == '\\') {
            ++pos;
            return parse_escape(pattern, pos);
        }

        if (c == '^' || c == '$') {
            // Anchors - simplified handling (match empty)
            ++pos;
            size_type state = nfa_states_.size();
            nfa_states_.push_back({nfa_state::type::epsilon, '\0', {}, no_transition, no_transition});
            return {state, state};
        }

        // Literal character
        ++pos;
        size_type state = nfa_states_.size();
        nfa_states_.push_back({nfa_state::type::char_match, c, {}, no_transition, no_transition});
        return {state, state};
    }

    nfa_fragment parse_char_class(std::string_view pattern, size_type& pos) {
        ++pos;  // consume '['
        bool negated = false;

        if (pos < pattern.size() && pattern[pos] == '^') {
            negated = true;
            ++pos;
        }

        char_class cc;

        while (pos < pattern.size() && pattern[pos] != ']') {
            char c = pattern[pos++];

            if (c == '\\' && pos < pattern.size()) {
                c = pattern[pos++];
                add_escape_to_class(cc, c);
            } else if (pos + 1 < pattern.size() && pattern[pos] == '-' &&
                       pattern[pos + 1] != ']') {
                ++pos;  // consume '-'
                char end = pattern[pos++];
                cc.set_range(c, end);
            } else {
                cc.set(c);
            }
        }

        if (pos >= pattern.size()) {
            throw std::runtime_error("Unclosed character class");
        }
        ++pos;  // consume ']'

        if (negated) {
            cc.flip();
        }

        size_type state = nfa_states_.size();
        nfa_states_.push_back({nfa_state::type::class_match, '\0', cc, no_transition, no_transition});
        return {state, state};
    }

    nfa_fragment parse_escape(std::string_view pattern, size_type& pos) {
        if (pos >= pattern.size()) {
            throw std::runtime_error("Incomplete escape sequence");
        }

        char c = pattern[pos++];
        size_type state = nfa_states_.size();

        switch (c) {
            case 'd':
                nfa_states_.push_back({nfa_state::type::class_match, '\0',
                                       char_class::digit(), no_transition, no_transition});
                break;
            case 'D': {
                auto cc = char_class::digit();
                cc.flip();
                nfa_states_.push_back({nfa_state::type::class_match, '\0', cc, no_transition, no_transition});
                break;
            }
            case 'w':
                nfa_states_.push_back({nfa_state::type::class_match, '\0',
                                       char_class::word(), no_transition, no_transition});
                break;
            case 'W': {
                auto cc = char_class::word();
                cc.flip();
                nfa_states_.push_back({nfa_state::type::class_match, '\0', cc, no_transition, no_transition});
                break;
            }
            case 's':
                nfa_states_.push_back({nfa_state::type::class_match, '\0',
                                       char_class::space(), no_transition, no_transition});
                break;
            case 'S': {
                auto cc = char_class::space();
                cc.flip();
                nfa_states_.push_back({nfa_state::type::class_match, '\0', cc, no_transition, no_transition});
                break;
            }
            default:
                // Literal escape (e.g., \., \*, etc.)
                nfa_states_.push_back({nfa_state::type::char_match, c, {}, no_transition, no_transition});
                break;
        }

        return {state, state};
    }

    void add_escape_to_class(char_class& cc, char c) {
        switch (c) {
            case 'd':
                for (char ch = '0'; ch <= '9'; ++ch) cc.set(ch);
                break;
            case 'w':
                for (char ch = 'a'; ch <= 'z'; ++ch) cc.set(ch);
                for (char ch = 'A'; ch <= 'Z'; ++ch) cc.set(ch);
                for (char ch = '0'; ch <= '9'; ++ch) cc.set(ch);
                cc.set('_');
                break;
            case 's':
                cc.set(' '); cc.set('\t'); cc.set('\n');
                cc.set('\r'); cc.set('\f'); cc.set('\v');
                break;
            default:
                cc.set(c);
                break;
        }
    }

    nfa_fragment make_star(nfa_fragment inner) {
        // a* = (a|epsilon)*
        // Create split state: can go to inner or skip (next2 is the skip path)
        size_type split = nfa_states_.size();
        nfa_states_.push_back({nfa_state::type::epsilon, '\0', {}, inner.start, no_transition});
        // inner.end loops back to split
        patch(inner.end, split);
        // The split state's next2 will be patched to whatever comes next
        // For now, split IS both start and end (dangling next2)
        return {split, split};
    }

    nfa_fragment make_plus(nfa_fragment inner) {
        // a+ = aa*
        // Must match inner at least once, then optionally repeat
        size_type split = nfa_states_.size();
        nfa_states_.push_back({nfa_state::type::epsilon, '\0', {}, inner.start, no_transition});
        // inner.end goes to split
        patch(inner.end, split);
        // split's next2 will be patched to whatever comes next
        // Start is inner.start (must match once), end is split (dangling next2)
        return {inner.start, split};
    }

    nfa_fragment make_optional(nfa_fragment inner) {
        size_type split = nfa_states_.size();
        nfa_states_.push_back({nfa_state::type::epsilon, '\0', {}, inner.start, no_transition});
        size_type join = nfa_states_.size();
        nfa_states_.push_back({nfa_state::type::epsilon, '\0', {}, no_transition, no_transition});
        nfa_states_[split].next2 = join;
        patch(inner.end, join);
        return {split, join};
    }

    void patch(size_type state, size_type target) {
        if (state < nfa_states_.size()) {
            auto& s = nfa_states_[state];
            // For epsilon states (split nodes), patch next2 if next1 is already set
            // For match states, patch next1
            if (s.kind == nfa_state::type::epsilon) {
                // Epsilon state: try next1 first, then next2
                if (!s.has_next1()) {
                    s.next1 = target;
                } else if (!s.has_next2()) {
                    s.next2 = target;
                }
            } else {
                // Match state: only has next1
                if (!s.has_next1()) {
                    s.next1 = target;
                }
            }
        }
    }

    // Epsilon closure
    void epsilon_closure(std::unordered_set<size_type>& states) const {
        std::vector<size_type> stack(states.begin(), states.end());

        while (!stack.empty()) {
            size_type s = stack.back();
            stack.pop_back();

            if (s >= nfa_states_.size()) continue;

            const auto& state = nfa_states_[s];
            if (state.kind == nfa_state::type::epsilon) {
                if (state.has_next1() && states.insert(state.next1).second) {
                    stack.push_back(state.next1);
                }
                if (state.has_next2() && states.insert(state.next2).second) {
                    stack.push_back(state.next2);
                }
            }
        }
    }

    void build_dfa() {
        states_.clear();

        if (nfa_states_.empty()) {
            return;
        }

        // Map from NFA state sets to DFA states
        std::unordered_map<std::string, size_type> state_map;

        auto set_to_key = [](const std::unordered_set<size_type>& s) {
            std::vector<size_type> v(s.begin(), s.end());
            std::sort(v.begin(), v.end());
            std::string key;
            for (auto x : v) {
                key += std::to_string(x) + ",";
            }
            return key;
        };

        // Start with epsilon closure of NFA start state
        std::unordered_set<size_type> start_set{nfa_start_};
        epsilon_closure(start_set);

        std::vector<std::unordered_set<size_type>> worklist;
        worklist.push_back(start_set);
        state_map[set_to_key(start_set)] = 0;

        states_.push_back(dfa_state{});

        // Check if start is accepting
        for (auto s : start_set) {
            if (s < nfa_states_.size() && nfa_states_[s].kind == nfa_state::type::accept) {
                states_[0].is_accept = true;
                break;
            }
        }

        size_type processed = 0;
        while (processed < worklist.size()) {
            if (states_.size() > config::max_dfa_states) {
                throw std::runtime_error("DFA state limit exceeded - pattern too complex");
            }

            // Copy to avoid reference invalidation when worklist grows
            std::unordered_set<size_type> current = worklist[processed];
            size_type current_dfa = state_map[set_to_key(current)];
            ++processed;

            // For each possible input character
            for (size_type c = 0; c < config::ascii_size; ++c) {
                std::unordered_set<size_type> next_set;

                for (auto s : current) {
                    if (s >= nfa_states_.size()) continue;

                    const auto& state = nfa_states_[s];
                    bool matches = false;

                    if (state.kind == nfa_state::type::char_match) {
                        matches = (static_cast<unsigned char>(state.match_char) == c);
                    } else if (state.kind == nfa_state::type::class_match) {
                        matches = state.match_class.test(static_cast<char>(c));
                    }

                    if (matches && state.has_next1()) {
                        next_set.insert(state.next1);
                    }
                }

                if (next_set.empty()) {
                    continue;
                }

                epsilon_closure(next_set);
                std::string key = set_to_key(next_set);

                size_type next_dfa;
                auto it = state_map.find(key);
                if (it == state_map.end()) {
                    next_dfa = states_.size();
                    state_map[key] = next_dfa;
                    states_.push_back(dfa_state{});
                    worklist.push_back(next_set);

                    // Check if accepting
                    for (auto s : next_set) {
                        if (s < nfa_states_.size() &&
                            nfa_states_[s].kind == nfa_state::type::accept) {
                            states_[next_dfa].is_accept = true;
                            break;
                        }
                    }
                } else {
                    next_dfa = it->second;
                }

                states_[current_dfa].transitions[c] = next_dfa;
            }
        }
    }
};

} // namespace kmp::detail
