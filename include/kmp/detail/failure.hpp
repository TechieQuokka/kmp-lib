#pragma once

/**
 * @file failure.hpp
 * @brief KMP failure function (prefix function) implementation
 *
 * The failure function is the heart of KMP algorithm. For each position i,
 * it computes the length of the longest proper prefix of pattern[0..i]
 * which is also a suffix.
 *
 * Time Complexity: O(m) where m is pattern length
 * Space Complexity: O(m)
 */

#include "../config.hpp"
#include <array>
#include <vector>
#include <span>
#include <concepts>
#include <iterator>

namespace kmp::detail {

/**
 * @brief Compute failure function for a pattern
 *
 * @tparam Iter Forward iterator type
 * @param first Iterator to pattern begin
 * @param last Iterator to pattern end
 * @return std::vector<size_type> Failure table
 *
 * Example:
 *   Pattern: "ABABAC"
 *   Failure: [0, 0, 1, 2, 3, 0]
 *
 *   Index:  0  1  2  3  4  5
 *   Char:   A  B  A  B  A  C
 *   Fail:   0  0  1  2  3  0
 */
template <std::forward_iterator Iter>
[[nodiscard]] constexpr auto compute_failure(Iter first, Iter last)
    -> std::vector<size_type>
{
    const auto m = static_cast<size_type>(std::distance(first, last));
    if (m == 0) {
        return {};
    }

    std::vector<size_type> failure(m, 0);

    // k = length of current longest proper prefix which is also suffix
    size_type k = 0;

    for (size_type i = 1; i < m; ++i) {
        auto pattern_i = std::next(first, static_cast<diff_type>(i));

        // Fall back until we find a match or k becomes 0
        while (k > 0 && *pattern_i != *std::next(first, static_cast<diff_type>(k))) {
            k = failure[k - 1];
        }

        // If we found a match, extend the prefix
        if (*pattern_i == *std::next(first, static_cast<diff_type>(k))) {
            ++k;
        }

        failure[i] = k;
    }

    return failure;
}

/**
 * @brief Optimized failure function with "nextval" optimization
 *
 * This is an improved version that skips unnecessary comparisons.
 * When pattern[i] == pattern[failure[i]], we can skip to failure[failure[i]].
 *
 * Reference: "Performance Comparison of Different Code Implementations
 *            of the KMP Algorithm" (2024)
 */
template <std::forward_iterator Iter>
[[nodiscard]] constexpr auto compute_failure_optimized(Iter first, Iter last)
    -> std::vector<size_type>
{
    const auto m = static_cast<size_type>(std::distance(first, last));
    if (m == 0) {
        return {};
    }

    std::vector<size_type> failure(m, 0);
    size_type k = 0;

    for (size_type i = 1; i < m; ++i) {
        auto pattern_i = std::next(first, static_cast<diff_type>(i));

        while (k > 0 && *pattern_i != *std::next(first, static_cast<diff_type>(k))) {
            k = failure[k - 1];
        }

        if (*pattern_i == *std::next(first, static_cast<diff_type>(k))) {
            ++k;
        }

        // Optimization: if pattern[i] == pattern[k], skip ahead
        if (k > 0 && i + 1 < m) {
            auto next_pattern = std::next(first, static_cast<diff_type>(i + 1));
            auto next_k = std::next(first, static_cast<diff_type>(k));
            if (*next_pattern == *next_k) {
                failure[i] = failure[k - 1];
                continue;
            }
        }

        failure[i] = k;
    }

    return failure;
}

/**
 * @brief Compile-time failure function for fixed patterns
 *
 * Enables zero-overhead pattern matching for known patterns at compile time.
 */
template <std::size_t N>
struct constexpr_failure {
    std::array<size_type, N> table{};

    consteval constexpr_failure(const char (&pattern)[N + 1]) {
        if constexpr (N == 0) {
            return;
        }

        size_type k = 0;
        for (size_type i = 1; i < N; ++i) {
            while (k > 0 && pattern[i] != pattern[k]) {
                k = table[k - 1];
            }
            if (pattern[i] == pattern[k]) {
                ++k;
            }
            table[i] = k;
        }
    }
};

/**
 * @brief Span-based failure function for contiguous memory
 *
 * More efficient for string_view and spans.
 */
[[nodiscard]] constexpr auto compute_failure(std::span<const char> pattern)
    -> std::vector<size_type>
{
    return compute_failure(pattern.begin(), pattern.end());
}

} // namespace kmp::detail
