#pragma once

/**
 * @file avx2.hpp
 * @brief AVX2 optimized string matching (256-bit / 32 bytes at once)
 *
 * AVX2 doubles the throughput compared to SSE4.2 by processing
 * 32 bytes simultaneously.
 */

#include "../../config.hpp"

#if KMP_HAS_AVX2 || defined(_MSC_VER)

#include <immintrin.h>
#include <cstring>

namespace kmp::detail::simd {

/**
 * @brief Find first occurrence of character using AVX2 (32 bytes/iteration)
 */
KMP_FORCE_INLINE const char* find_first_char_avx2(
    const char* haystack,
    size_type haystack_len,
    char needle_char
) noexcept {
    if (haystack_len == 0) {
        return nullptr;
    }

    // Broadcast needle character to all 32 bytes
    const __m256i needle = _mm256_set1_epi8(needle_char);

    const char* ptr = haystack;
    const char* end = haystack + haystack_len;

    // Process 32 bytes at a time
    while (ptr + 32 <= end) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr));
        __m256i cmp = _mm256_cmpeq_epi8(chunk, needle);
        int mask = _mm256_movemask_epi8(cmp);

        if (mask != 0) {
            #if defined(_MSC_VER)
                unsigned long idx;
                _BitScanForward(&idx, static_cast<unsigned long>(mask));
                return ptr + idx;
            #else
                return ptr + __builtin_ctz(static_cast<unsigned>(mask));
            #endif
        }
        ptr += 32;
    }

    // Handle remaining with SSE or scalar
    while (ptr + 16 <= end) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));
        __m128i needle_128 = _mm_set1_epi8(needle_char);
        __m128i cmp = _mm_cmpeq_epi8(chunk, needle_128);
        int mask = _mm_movemask_epi8(cmp);

        if (mask != 0) {
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

    // Scalar fallback for remaining bytes
    while (ptr < end) {
        if (*ptr == needle_char) {
            return ptr;
        }
        ++ptr;
    }

    return nullptr;
}

/**
 * @brief Compare two strings using AVX2 for quick mismatch detection
 *
 * Returns the position of first mismatch, or length if equal.
 */
KMP_FORCE_INLINE size_type compare_avx2(
    const char* a,
    const char* b,
    size_type len
) noexcept {
    size_type i = 0;

    // Compare 32 bytes at a time
    while (i + 32 <= len) {
        __m256i va = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(a + i));
        __m256i vb = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(b + i));
        __m256i cmp = _mm256_cmpeq_epi8(va, vb);
        int mask = _mm256_movemask_epi8(cmp);

        if (mask != static_cast<int>(0xFFFFFFFF)) {
            // Found mismatch
            #if defined(_MSC_VER)
                unsigned long idx;
                _BitScanForward(&idx, static_cast<unsigned long>(~mask));
                return i + idx;
            #else
                return i + __builtin_ctz(static_cast<unsigned>(~mask));
            #endif
        }
        i += 32;
    }

    // Scalar fallback
    while (i < len) {
        if (a[i] != b[i]) {
            return i;
        }
        ++i;
    }

    return len;
}

/**
 * @brief AVX2 accelerated KMP search
 */
template <typename FailureTable>
KMP_FORCE_INLINE const char* kmp_search_avx2(
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
        // AVX2 scan for first character
        size_type remaining = static_cast<size_type>(text_end - text_ptr);
        const char* match = find_first_char_avx2(text_ptr, remaining, first_char);

        if (!match) {
            return nullptr;
        }

        // Use AVX2 compare for pattern verification
        size_type match_len = compare_avx2(match, pattern, pattern_len);

        if (match_len == pattern_len) {
            return match;
        }

        // Skip using failure function
        size_type skip = 1;
        if (match_len > 0) {
            skip = match_len - failure[match_len - 1];
        }

        text_ptr = match + skip;
    }

    return nullptr;
}

/**
 * @brief Find all occurrences using AVX2
 */
template <typename FailureTable, typename OutputIt>
KMP_FORCE_INLINE OutputIt kmp_search_all_avx2(
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
        const char* match = kmp_search_avx2(
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
        pos = match + 1;
    }

    return out;
}

} // namespace kmp::detail::simd

#endif // KMP_HAS_AVX2
