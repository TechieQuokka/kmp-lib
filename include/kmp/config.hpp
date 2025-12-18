#pragma once

/**
 * @file config.hpp
 * @brief KMP Library configuration and compile-time settings
 *
 * This header provides compile-time configuration options for the KMP library,
 * including SIMD detection, platform-specific settings, and feature flags.
 */

#include <cstdint>
#include <cstddef>

namespace kmp {

// Version info
inline constexpr int version_major = 1;
inline constexpr int version_minor = 0;
inline constexpr int version_patch = 0;

// =============================================================================
// SIMD Detection
// =============================================================================

#if defined(__AVX512F__) && defined(__AVX512BW__)
    #define KMP_HAS_AVX512 1
#else
    #define KMP_HAS_AVX512 0
#endif

#if defined(__AVX2__)
    #define KMP_HAS_AVX2 1
#else
    #define KMP_HAS_AVX2 0
#endif

#if defined(__SSE4_2__)
    #define KMP_HAS_SSE42 1
#else
    #define KMP_HAS_SSE42 0
#endif

// SIMD width in bytes
#if KMP_HAS_AVX512
    inline constexpr std::size_t simd_width = 64;
#elif KMP_HAS_AVX2
    inline constexpr std::size_t simd_width = 32;
#elif KMP_HAS_SSE42
    inline constexpr std::size_t simd_width = 16;
#else
    inline constexpr std::size_t simd_width = 1;  // scalar fallback
#endif

// =============================================================================
// Platform Detection
// =============================================================================

#if defined(_MSC_VER)
    #define KMP_MSVC 1
    #define KMP_FORCE_INLINE __forceinline
    #define KMP_NOINLINE __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
    #define KMP_GCC_CLANG 1
    #define KMP_FORCE_INLINE __attribute__((always_inline)) inline
    #define KMP_NOINLINE __attribute__((noinline))
#else
    #define KMP_FORCE_INLINE inline
    #define KMP_NOINLINE
#endif

// Likely/Unlikely hints
#if defined(__cplusplus) && __cplusplus >= 202002L
    #define KMP_LIKELY(x)   (x) [[likely]]
    #define KMP_UNLIKELY(x) (x) [[unlikely]]
#elif defined(__GNUC__)
    #define KMP_LIKELY(x)   (__builtin_expect(!!(x), 1))
    #define KMP_UNLIKELY(x) (__builtin_expect(!!(x), 0))
#else
    #define KMP_LIKELY(x)   (x)
    #define KMP_UNLIKELY(x) (x)
#endif

// =============================================================================
// Configuration Constants
// =============================================================================

namespace config {

// Minimum text length to use SIMD
inline constexpr std::size_t simd_threshold = 64;

// Maximum pattern length for stack allocation
inline constexpr std::size_t max_stack_pattern = 256;

// DFA state limit (to prevent exponential blowup)
inline constexpr std::size_t max_dfa_states = 10000;

// ASCII charset size
inline constexpr std::size_t ascii_size = 128;

} // namespace config

// =============================================================================
// Type Aliases
// =============================================================================

using size_type = std::size_t;
using diff_type = std::ptrdiff_t;

} // namespace kmp
