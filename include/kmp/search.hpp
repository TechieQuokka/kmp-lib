#pragma once

/**
 * @file search.hpp
 * @brief STL-compatible iterator-based search API
 *
 * Provides std::search-compatible interface with KMP algorithm
 * and automatic SIMD dispatch for optimal performance.
 *
 * Usage:
 *   auto it = kmp::search(text.begin(), text.end(),
 *                         pattern.begin(), pattern.end());
 */

#include "config.hpp"
#include "detail/failure.hpp"
#include "detail/simd/dispatch.hpp"

#if KMP_HAS_AVX512
    #include "detail/simd/avx512.hpp"
#endif
#if KMP_HAS_AVX2
    #include "detail/simd/avx2.hpp"
#endif
#if KMP_HAS_SSE42
    #include "detail/simd/sse42.hpp"
#endif

#include <iterator>
#include <concepts>
#include <ranges>
#include <string_view>
#include <span>
#include <vector>
#include <optional>
#include <generator>

namespace kmp {

// =============================================================================
// Concepts
// =============================================================================

template <typename Iter>
concept char_iterator = std::forward_iterator<Iter> &&
    std::same_as<std::remove_cvref_t<std::iter_value_t<Iter>>, char>;

template <typename Iter>
concept random_access_char_iterator =
    std::random_access_iterator<Iter> && char_iterator<Iter>;

template <typename Iter>
concept contiguous_char_iterator =
    std::contiguous_iterator<Iter> && char_iterator<Iter>;

// =============================================================================
// Scalar KMP Implementation
// =============================================================================

namespace detail {

/**
 * @brief Pure scalar KMP search (fallback)
 */
template <std::forward_iterator TextIter, std::forward_iterator PatternIter>
[[nodiscard]] TextIter kmp_search_scalar(
    TextIter text_first,
    TextIter text_last,
    PatternIter pattern_first,
    PatternIter pattern_last,
    const std::vector<size_type>& failure
) {
    const auto m = static_cast<size_type>(std::distance(pattern_first, pattern_last));
    if (m == 0) {
        return text_first;
    }

    size_type j = 0;  // pattern index

    for (auto it = text_first; it != text_last; ++it) {
        while (j > 0 && *it != *std::next(pattern_first, static_cast<diff_type>(j))) {
            j = failure[j - 1];
        }

        if (*it == *std::next(pattern_first, static_cast<diff_type>(j))) {
            ++j;
        }

        if (j == m) {
            // Found match - return iterator to start of match
            return std::next(text_first,
                std::distance(text_first, it) - static_cast<diff_type>(m) + 1);
        }
    }

    return text_last;
}

} // namespace detail

// =============================================================================
// Main Search Function
// =============================================================================

/**
 * @brief Search for pattern in text using KMP algorithm with SIMD acceleration
 *
 * @tparam TextIter Forward iterator type for text
 * @tparam PatternIter Forward iterator type for pattern
 * @param text_first Iterator to text begin
 * @param text_last Iterator to text end
 * @param pattern_first Iterator to pattern begin
 * @param pattern_last Iterator to pattern end
 * @return Iterator to first match, or text_last if not found
 *
 * Complexity: O(n + m) time, O(m) space
 */
template <std::forward_iterator TextIter, std::forward_iterator PatternIter>
[[nodiscard]] TextIter search(
    TextIter text_first,
    TextIter text_last,
    PatternIter pattern_first,
    PatternIter pattern_last
) {
    const auto n = static_cast<size_type>(std::distance(text_first, text_last));
    const auto m = static_cast<size_type>(std::distance(pattern_first, pattern_last));

    if (m == 0) {
        return text_first;
    }
    if (n < m) {
        return text_last;
    }

    // Compute failure function
    auto failure = detail::compute_failure(pattern_first, pattern_last);

    // For contiguous char iterators, use SIMD acceleration
    if constexpr (contiguous_char_iterator<TextIter> &&
                  contiguous_char_iterator<PatternIter>) {

        const char* text_ptr = std::to_address(text_first);
        const char* pattern_ptr = std::to_address(pattern_first);

        const char* result = nullptr;

        // Runtime SIMD dispatch
        if (n >= config::simd_threshold) {
            #if KMP_HAS_AVX512
            if (detail::simd::has_avx512()) {
                result = detail::simd::kmp_search_avx512(
                    text_ptr, n, pattern_ptr, m, failure);
            } else
            #endif
            #if KMP_HAS_AVX2
            if (detail::simd::has_avx2()) {
                result = detail::simd::kmp_search_avx2(
                    text_ptr, n, pattern_ptr, m, failure);
            } else
            #endif
            #if KMP_HAS_SSE42
            if (detail::simd::has_sse42()) {
                result = detail::simd::kmp_search_sse42(
                    text_ptr, n, pattern_ptr, m, failure);
            } else
            #endif
            {
                // Scalar fallback
                return detail::kmp_search_scalar(
                    text_first, text_last, pattern_first, pattern_last, failure);
            }

            if (result) {
                return text_first + (result - text_ptr);
            }
            return text_last;
        }
    }

    // Scalar fallback for non-contiguous or small inputs
    return detail::kmp_search_scalar(
        text_first, text_last, pattern_first, pattern_last, failure);
}

// =============================================================================
// Convenience Overloads
// =============================================================================

/**
 * @brief Search using string_view
 */
[[nodiscard]] inline std::string_view::iterator search(
    std::string_view text,
    std::string_view pattern
) {
    auto it = search(text.begin(), text.end(), pattern.begin(), pattern.end());
    return it;
}

/**
 * @brief Search returning position instead of iterator
 */
[[nodiscard]] inline std::optional<size_type> search_pos(
    std::string_view text,
    std::string_view pattern
) {
    auto it = search(text.begin(), text.end(), pattern.begin(), pattern.end());
    if (it == text.end()) {
        return std::nullopt;
    }
    return static_cast<size_type>(std::distance(text.begin(), it));
}

// =============================================================================
// Search All (Generator-based)
// =============================================================================

/**
 * @brief Find all occurrences of pattern in text
 *
 * Returns a C++23 generator that yields positions of all matches.
 * Allows overlapping matches.
 *
 * @return std::generator<size_type> yielding match positions
 */
[[nodiscard]] inline std::generator<size_type> search_all(
    std::string_view text,
    std::string_view pattern
) {
    const auto n = text.size();
    const auto m = pattern.size();

    if (m == 0 || n < m) {
        co_return;
    }

    auto failure = detail::compute_failure(pattern.begin(), pattern.end());
    size_type j = 0;

    for (size_type i = 0; i < n; ++i) {
        while (j > 0 && text[i] != pattern[j]) {
            j = failure[j - 1];
        }

        if (text[i] == pattern[j]) {
            ++j;
        }

        if (j == m) {
            co_yield i - m + 1;
            j = failure[j - 1];  // Continue searching (overlapping)
        }
    }
}

/**
 * @brief Find all occurrences and return as vector
 */
[[nodiscard]] inline std::vector<size_type> search_all_vec(
    std::string_view text,
    std::string_view pattern
) {
    std::vector<size_type> results;
    for (auto pos : search_all(text, pattern)) {
        results.push_back(pos);
    }
    return results;
}

// =============================================================================
// Count Matches
// =============================================================================

/**
 * @brief Count occurrences of pattern in text
 */
[[nodiscard]] inline size_type count(
    std::string_view text,
    std::string_view pattern
) {
    size_type result = 0;
    for ([[maybe_unused]] auto _ : search_all(text, pattern)) {
        ++result;
    }
    return result;
}

// =============================================================================
// Contains Check
// =============================================================================

/**
 * @brief Check if text contains pattern
 */
[[nodiscard]] inline bool contains(
    std::string_view text,
    std::string_view pattern
) {
    return search_pos(text, pattern).has_value();
}

} // namespace kmp
