#pragma once

/**
 * @file pattern.hpp
 * @brief Pattern types for literal and regex matching
 *
 * Provides:
 *   - literal_pattern: For exact string matching (pure KMP)
 *   - regex_pattern: For regex matching (DFA engine)
 *   - compile<"pattern">(): Compile-time pattern
 */

#include "config.hpp"
#include "detail/failure.hpp"
#include "detail/dfa.hpp"

#include <string>
#include <string_view>
#include <memory>
#include <utility>

namespace kmp {

// =============================================================================
// Literal Pattern (Pure KMP)
// =============================================================================

/**
 * @brief Compiled literal pattern for repeated searches
 *
 * Pre-computes the failure function once, allowing efficient
 * multiple searches with the same pattern.
 */
class literal_pattern {
public:
    literal_pattern() = default;

    explicit literal_pattern(const char* pattern)
        : pattern_(pattern)
        , failure_(detail::compute_failure(pattern_.begin(), pattern_.end()))
    {}

    explicit literal_pattern(std::string pattern)
        : pattern_(std::move(pattern))
        , failure_(detail::compute_failure(pattern_.begin(), pattern_.end()))
    {}

    explicit literal_pattern(std::string_view pattern)
        : pattern_(pattern)
        , failure_(detail::compute_failure(pattern_.begin(), pattern_.end()))
    {}

    [[nodiscard]] std::string_view pattern() const noexcept {
        return pattern_;
    }

    [[nodiscard]] const std::vector<size_type>& failure() const noexcept {
        return failure_;
    }

    [[nodiscard]] size_type size() const noexcept {
        return pattern_.size();
    }

    [[nodiscard]] bool empty() const noexcept {
        return pattern_.empty();
    }

    [[nodiscard]] char operator[](size_type i) const noexcept {
        return pattern_[i];
    }

    [[nodiscard]] auto begin() const noexcept { return pattern_.begin(); }
    [[nodiscard]] auto end() const noexcept { return pattern_.end(); }

private:
    std::string pattern_;
    std::vector<size_type> failure_;
};

// =============================================================================
// Regex Pattern (DFA Engine)
// =============================================================================

/**
 * @brief Compiled regex pattern with O(n) matching guarantee
 *
 * Compiles a regex into a DFA for linear-time matching.
 * Thread-safe for concurrent searches.
 */
class regex_pattern {
public:
    regex_pattern() = default;

    explicit regex_pattern(std::string_view pattern)
        : source_(pattern)
        , dfa_(std::make_shared<detail::compiled_dfa>(pattern))
    {}

    [[nodiscard]] std::string_view source() const noexcept {
        return source_;
    }

    [[nodiscard]] std::optional<size_type> search(std::string_view text) const {
        if (!dfa_) return std::nullopt;
        return dfa_->search(text);
    }

    [[nodiscard]] bool matches(std::string_view text) const {
        if (!dfa_) return false;
        return dfa_->matches(text);
    }

    [[nodiscard]] bool empty() const noexcept {
        return !dfa_ || dfa_->empty();
    }

    [[nodiscard]] size_type state_count() const noexcept {
        return dfa_ ? dfa_->state_count() : 0;
    }

private:
    std::string source_;
    std::shared_ptr<detail::compiled_dfa> dfa_;
};

// =============================================================================
// Compile-time Pattern (constexpr)
// =============================================================================

/**
 * @brief Compile-time string for pattern matching
 */
template <size_type N>
struct fixed_string {
    char data[N]{};

    consteval fixed_string(const char (&str)[N]) {
        for (size_type i = 0; i < N; ++i) {
            data[i] = str[i];
        }
    }

    [[nodiscard]] constexpr size_type size() const noexcept { return N - 1; }
    [[nodiscard]] constexpr const char* c_str() const noexcept { return data; }
    [[nodiscard]] constexpr std::string_view view() const noexcept {
        return {data, N - 1};
    }
};

/**
 * @brief Compile-time compiled pattern
 *
 * The failure function is computed at compile time, enabling
 * zero-overhead pattern matching.
 */
template <fixed_string Pattern>
class compiled_pattern {
public:
    static constexpr size_type pattern_size = Pattern.size();

    consteval compiled_pattern() {
        compute_failure();
    }

    [[nodiscard]] static constexpr std::string_view pattern() noexcept {
        return Pattern.view();
    }

    [[nodiscard]] static constexpr size_type size() noexcept {
        return pattern_size;
    }

    [[nodiscard]] constexpr size_type failure_at(size_type i) const noexcept {
        return failure_[i];
    }

    [[nodiscard]] constexpr const auto& failure() const noexcept {
        return failure_;
    }

    [[nodiscard]] constexpr char operator[](size_type i) const noexcept {
        return Pattern.data[i];
    }

private:
    std::array<size_type, pattern_size> failure_{};

    consteval void compute_failure() {
        if constexpr (pattern_size == 0) {
            return;
        }

        failure_[0] = 0;
        size_type k = 0;

        for (size_type i = 1; i < pattern_size; ++i) {
            while (k > 0 && Pattern.data[i] != Pattern.data[k]) {
                k = failure_[k - 1];
            }
            if (Pattern.data[i] == Pattern.data[k]) {
                ++k;
            }
            failure_[i] = k;
        }
    }
};

/**
 * @brief Create a compile-time pattern
 *
 * Usage:
 *   constexpr auto pat = kmp::compile<"hello">();
 *   auto it = kmp::search(text.begin(), text.end(), pat);
 */
template <fixed_string Pattern>
[[nodiscard]] consteval auto compile() {
    return compiled_pattern<Pattern>{};
}

// =============================================================================
// Compile Function (Runtime)
// =============================================================================

/**
 * @brief Compile a literal pattern at runtime
 */
[[nodiscard]] inline literal_pattern compile_literal(std::string_view pattern) {
    return literal_pattern{pattern};
}

/**
 * @brief Compile a regex pattern at runtime
 */
[[nodiscard]] inline regex_pattern compile_regex(std::string_view pattern) {
    return regex_pattern{pattern};
}

// =============================================================================
// Search with Compiled Pattern
// =============================================================================

/**
 * @brief Search with pre-compiled literal pattern
 */
template <std::forward_iterator Iter>
[[nodiscard]] Iter search(Iter first, Iter last, const literal_pattern& pattern);

/**
 * @brief Search with compile-time pattern
 */
template <std::forward_iterator Iter, fixed_string Pattern>
[[nodiscard]] Iter search(Iter first, Iter last, const compiled_pattern<Pattern>& pattern);

} // namespace kmp
