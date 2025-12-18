#pragma once

/**
 * @file dispatch.hpp
 * @brief Runtime SIMD dispatch and CPU feature detection
 *
 * Provides runtime detection of CPU capabilities and dispatches
 * to the optimal SIMD implementation.
 */

#include "../../config.hpp"
#include <cstdint>
#include <array>

#if defined(_MSC_VER)
    #include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
    #include <cpuid.h>
#endif

namespace kmp::detail::simd {

// =============================================================================
// CPU Feature Flags
// =============================================================================

enum class cpu_feature : std::uint32_t {
    none    = 0,
    sse42   = 1 << 0,
    avx2    = 1 << 1,
    avx512f = 1 << 2,
    avx512bw = 1 << 3,
};

[[nodiscard]] constexpr cpu_feature operator|(cpu_feature a, cpu_feature b) noexcept {
    return static_cast<cpu_feature>(
        static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b)
    );
}

[[nodiscard]] constexpr cpu_feature operator&(cpu_feature a, cpu_feature b) noexcept {
    return static_cast<cpu_feature>(
        static_cast<std::uint32_t>(a) & static_cast<std::uint32_t>(b)
    );
}

[[nodiscard]] constexpr bool has_feature(cpu_feature flags, cpu_feature test) noexcept {
    return (flags & test) == test;
}

// =============================================================================
// CPUID Wrapper
// =============================================================================

namespace detail {

struct cpuid_result {
    std::uint32_t eax, ebx, ecx, edx;
};

inline cpuid_result cpuid(std::uint32_t leaf, std::uint32_t subleaf = 0) noexcept {
    cpuid_result result{};
#if defined(_MSC_VER)
    std::array<int, 4> regs{};
    __cpuidex(regs.data(), static_cast<int>(leaf), static_cast<int>(subleaf));
    result.eax = static_cast<std::uint32_t>(regs[0]);
    result.ebx = static_cast<std::uint32_t>(regs[1]);
    result.ecx = static_cast<std::uint32_t>(regs[2]);
    result.edx = static_cast<std::uint32_t>(regs[3]);
#elif defined(__GNUC__) || defined(__clang__)
    __cpuid_count(leaf, subleaf, result.eax, result.ebx, result.ecx, result.edx);
#endif
    return result;
}

inline std::uint64_t xgetbv(std::uint32_t xcr) noexcept {
#if defined(_MSC_VER)
    return _xgetbv(xcr);
#elif defined(__GNUC__) || defined(__clang__)
    std::uint32_t eax, edx;
    __asm__ volatile("xgetbv" : "=a"(eax), "=d"(edx) : "c"(xcr));
    return (static_cast<std::uint64_t>(edx) << 32) | eax;
#else
    return 0;
#endif
}

} // namespace detail

// =============================================================================
// Feature Detection
// =============================================================================

/**
 * @brief Detect available CPU features at runtime
 */
[[nodiscard]] inline cpu_feature detect_features() noexcept {
    cpu_feature features = cpu_feature::none;

    // Get max leaf
    auto [max_leaf, vendor_ebx, vendor_ecx, vendor_edx] = detail::cpuid(0);
    if (max_leaf < 1) {
        return features;
    }

    // Leaf 1: SSE4.2
    auto [eax1, ebx1, ecx1, edx1] = detail::cpuid(1);

    // SSE4.2: ECX bit 20
    if (ecx1 & (1u << 20)) {
        features = features | cpu_feature::sse42;
    }

    // Check OSXSAVE (ECX bit 27) for AVX support
    if (!(ecx1 & (1u << 27))) {
        return features;
    }

    // Check XCR0 for OS support of YMM (AVX) and ZMM (AVX-512)
    std::uint64_t xcr0 = detail::xgetbv(0);
    bool os_avx = (xcr0 & 0x6) == 0x6;           // XMM + YMM
    bool os_avx512 = (xcr0 & 0xE6) == 0xE6;      // XMM + YMM + ZMM + opmask

    if (!os_avx) {
        return features;
    }

    // Leaf 7: AVX2 and AVX-512
    if (max_leaf >= 7) {
        auto [eax7, ebx7, ecx7, edx7] = detail::cpuid(7, 0);

        // AVX2: EBX bit 5
        if (ebx7 & (1u << 5)) {
            features = features | cpu_feature::avx2;
        }

        // AVX-512F: EBX bit 16
        if (os_avx512 && (ebx7 & (1u << 16))) {
            features = features | cpu_feature::avx512f;

            // AVX-512BW: EBX bit 30
            if (ebx7 & (1u << 30)) {
                features = features | cpu_feature::avx512bw;
            }
        }
    }

    return features;
}

/**
 * @brief Get cached CPU features (initialized once)
 */
[[nodiscard]] inline cpu_feature get_features() noexcept {
    static const cpu_feature cached = detect_features();
    return cached;
}

/**
 * @brief Check if AVX-512 with BW is available
 */
[[nodiscard]] inline bool has_avx512() noexcept {
    return has_feature(get_features(), cpu_feature::avx512f | cpu_feature::avx512bw);
}

/**
 * @brief Check if AVX2 is available
 */
[[nodiscard]] inline bool has_avx2() noexcept {
    return has_feature(get_features(), cpu_feature::avx2);
}

/**
 * @brief Check if SSE4.2 is available
 */
[[nodiscard]] inline bool has_sse42() noexcept {
    return has_feature(get_features(), cpu_feature::sse42);
}

// =============================================================================
// SIMD Level Enum
// =============================================================================

enum class simd_level {
    scalar,
    sse42,
    avx2,
    avx512
};

/**
 * @brief Get the best available SIMD level
 */
[[nodiscard]] inline simd_level get_simd_level() noexcept {
    if (has_avx512()) return simd_level::avx512;
    if (has_avx2()) return simd_level::avx2;
    if (has_sse42()) return simd_level::sse42;
    return simd_level::scalar;
}

} // namespace kmp::detail::simd
