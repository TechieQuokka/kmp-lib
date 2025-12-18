#pragma once

/**
 * @file sse42.hpp
 * @brief SSE4.2 optimized string matching
 *
 * Uses PCMPESTRI/PCMPESTRM instructions for accelerated first-character
 * scanning. SSE4.2 provides hardware support for string operations.
 */

#include "../../config.hpp"

#if KMP_HAS_SSE42 || defined(_MSC_VER)

#include <nmmintrin.h>  // SSE4.2
#include <cstring>

namespace kmp::detail::simd {

/**
 * @brief Find first occurrence of any character from needle in haystack
 *
 * Uses PCMPESTRI for parallel character comparison.
 */
KMP_FORCE_INLINE const char* find_first_char_sse42(
    const char* haystack,
    size_type haystack_len,
    char needle_char
) noexcept {
    if (haystack_len == 0) {
        return nullptr;
    }

    // Broadcast needle character to all 16 bytes
    const __m128i needle = _mm_set1_epi8(needle_char);

    const char* ptr = haystack;
    const char* end = haystack + haystack_len;

    // Process 16 bytes at a time
    while (ptr + 16 <= end) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));
        __m128i cmp = _mm_cmpeq_epi8(chunk, needle);
        int mask = _mm_movemask_epi8(cmp);

        if (mask != 0) {
            // Found match, return position of first set bit
            #if defined(_MSC_VER)
                unsigned long idx;
                _BitScanForward(&idx, static_cast<unsigned long>(mask));
                return ptr + idx;
            #else
                return ptr + __builtin_ctz(static_cast<unsigned>(mask));
            #endif
        }
        ptr += 16;
    }

    // Handle remaining bytes
    while (ptr < end) {
        if (*ptr == needle_char) {
            return ptr;
        }
        ++ptr;
    }

    return nullptr;
}

/**
 * @brief SSE4.2 accelerated KMP search
 *
 * Strategy:
 * 1. Use SIMD to quickly scan for first character matches
 * 2. When found, verify full pattern using scalar KMP
 */
template <typename FailureTable>
KMP_FORCE_INLINE const char* kmp_search_sse42(
    const char* text,
    size_type text_len,
    const char* pattern,
    size_type pattern_len,
    const FailureTable& failure
) noexcept {
    if (pattern_len == 0) {
        return text;
    }
    if (text_len < pattern_len) {
        return nullptr;
    }

    const char first_char = pattern[0];
    const char* text_ptr = text;
    const char* text_end = text + text_len - pattern_len + 1;

    while (text_ptr < text_end) {
        // SIMD scan for first character
        size_type remaining = static_cast<size_type>(text_end - text_ptr);
        const char* match = find_first_char_sse42(text_ptr, remaining, first_char);

        if (!match) {
            return nullptr;
        }

        // Verify full pattern match
        size_type j = 1;
        while (j < pattern_len && match[j] == pattern[j]) {
            ++j;
        }

        if (j == pattern_len) {
            return match;
        }

        // Use failure function to skip
        while (j > 0 && match[j] != pattern[j]) {
            j = failure[j - 1];
        }

        text_ptr = match + 1;
    }

    return nullptr;
}

/**
 * @brief Find all occurrences using SSE4.2
 */
template <typename FailureTable, typename OutputIt>
KMP_FORCE_INLINE OutputIt kmp_search_all_sse42(
    const char* text,
    size_type text_len,
    const char* pattern,
    size_type pattern_len,
    const FailureTable& failure,
    OutputIt out
) noexcept {
    if (pattern_len == 0 || text_len < pattern_len) {
        return out;
    }

    const char* pos = text;
    const char* end = text + text_len;

    while (pos <= end - pattern_len) {
        const char* match = kmp_search_sse42(
            pos,
            static_cast<size_type>(end - pos),
            pattern,
            pattern_len,
            failure
        );

        if (!match) {
            break;
        }

        *out++ = static_cast<size_type>(match - text);
        pos = match + 1;  // Allow overlapping matches
    }

    return out;
}

} // namespace kmp::detail::simd

#endif // KMP_HAS_SSE42
