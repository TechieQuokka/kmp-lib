#pragma once

/**
 * @file avx512.hpp
 * @brief AVX-512 optimized string matching (512-bit / 64 bytes at once)
 *
 * AVX-512 provides the highest throughput by processing 64 bytes
 * in a single instruction. Requires AVX-512F and AVX-512BW.
 */

#include "../../config.hpp"

#if KMP_HAS_AVX512 || (defined(_MSC_VER) && defined(__AVX512F__))

#include <immintrin.h>
#include <cstring>

namespace kmp::detail::simd {

/**
 * @brief Find first occurrence of character using AVX-512 (64 bytes/iteration)
 */
KMP_FORCE_INLINE const char* find_first_char_avx512(
    const char* haystack,
    size_type haystack_len,
    char needle_char
) noexcept {
    if (haystack_len == 0) {
        return nullptr;
    }

    // Broadcast needle character to all 64 bytes
    const __m512i needle = _mm512_set1_epi8(needle_char);

    const char* ptr = haystack;
    const char* end = haystack + haystack_len;

    // Process 64 bytes at a time
    while (ptr + 64 <= end) {
        __m512i chunk = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(ptr));
        __mmask64 mask = _mm512_cmpeq_epi8_mask(chunk, needle);

        if (mask != 0) {
            #if defined(_MSC_VER)
                unsigned long idx;
                #if defined(_M_X64)
                    _BitScanForward64(&idx, mask);
                #else
                    if (static_cast<unsigned long>(mask) != 0) {
                        _BitScanForward(&idx, static_cast<unsigned long>(mask));
                    } else {
                        _BitScanForward(&idx, static_cast<unsigned long>(mask >> 32));
                        idx += 32;
                    }
                #endif
                return ptr + idx;
            #else
                return ptr + __builtin_ctzll(mask);
            #endif
        }
        ptr += 64;
    }

    // Handle remaining 32 bytes with AVX2
    if (ptr + 32 <= end) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr));
        __m256i needle_256 = _mm256_set1_epi8(needle_char);
        __m256i cmp = _mm256_cmpeq_epi8(chunk, needle_256);
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

    // Handle remaining 16 bytes with SSE
    if (ptr + 16 <= end) {
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
 * @brief Compare two strings using AVX-512 for quick mismatch detection
 */
KMP_FORCE_INLINE size_type compare_avx512(
    const char* a,
    const char* b,
    size_type len
) noexcept {
    size_type i = 0;

    // Compare 64 bytes at a time
    while (i + 64 <= len) {
        __m512i va = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(a + i));
        __m512i vb = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(b + i));
        __mmask64 eq_mask = _mm512_cmpeq_epi8_mask(va, vb);

        if (eq_mask != 0xFFFFFFFFFFFFFFFFULL) {
            // Found mismatch
            __mmask64 neq_mask = ~eq_mask;
            #if defined(_MSC_VER)
                unsigned long idx;
                #if defined(_M_X64)
                    _BitScanForward64(&idx, neq_mask);
                #else
                    if (static_cast<unsigned long>(neq_mask) != 0) {
                        _BitScanForward(&idx, static_cast<unsigned long>(neq_mask));
                    } else {
                        _BitScanForward(&idx, static_cast<unsigned long>(neq_mask >> 32));
                        idx += 32;
                    }
                #endif
                return i + idx;
            #else
                return i + __builtin_ctzll(neq_mask);
            #endif
        }
        i += 64;
    }

    // Compare remaining 32 bytes
    if (i + 32 <= len) {
        __m256i va = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(a + i));
        __m256i vb = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(b + i));
        __m256i cmp = _mm256_cmpeq_epi8(va, vb);
        int mask = _mm256_movemask_epi8(cmp);

        if (mask != static_cast<int>(0xFFFFFFFF)) {
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
 * @brief AVX-512 accelerated KMP search
 */
template <typename FailureTable>
KMP_FORCE_INLINE const char* kmp_search_avx512(
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
        // AVX-512 scan for first character
        size_type remaining = static_cast<size_type>(text_end - text_ptr);
        const char* match = find_first_char_avx512(text_ptr, remaining, first_char);

        if (!match) {
            return nullptr;
        }

        // Use AVX-512 compare for pattern verification
        size_type match_len = compare_avx512(match, pattern, pattern_len);

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
 * @brief Find all occurrences using AVX-512
 */
template <typename FailureTable, typename OutputIt>
KMP_FORCE_INLINE OutputIt kmp_search_all_avx512(
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
        const char* match = kmp_search_avx512(
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

#endif // KMP_HAS_AVX512
