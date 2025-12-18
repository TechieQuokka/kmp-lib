/**
 * @file bench_simd.cpp
 * @brief Benchmarks for SIMD acceleration
 */

#include <benchmark/benchmark.h>
#include <kmp/kmp.hpp>
#include <kmp/detail/simd/dispatch.hpp>
#include <string>
#include <random>

namespace {

std::string generate_text(size_t length, unsigned seed = 42) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> dis('a', 'z');

    std::string result;
    result.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        result += static_cast<char>(dis(gen));
    }
    return result;
}

} // namespace

// =============================================================================
// SIMD Level Information
// =============================================================================

static void BM_SIMD_Level_Check(benchmark::State& state) {
    for (auto _ : state) {
        auto level = kmp::detail::simd::get_simd_level();
        benchmark::DoNotOptimize(level);
    }
}

BENCHMARK(BM_SIMD_Level_Check);

// =============================================================================
// Text Size Scaling
// =============================================================================

static void BM_SIMD_Scaling(benchmark::State& state) {
    const size_t text_len = static_cast<size_t>(state.range(0));
    std::string text = generate_text(text_len);
    std::string pattern = "findme";

    // Insert pattern at 3/4 position
    text.replace(text_len * 3 / 4, pattern.size(), pattern);

    for (auto _ : state) {
        auto result = kmp::search_pos(text, pattern);
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                           static_cast<int64_t>(text_len));
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

BENCHMARK(BM_SIMD_Scaling)
    ->RangeMultiplier(2)
    ->Range(64, 1 << 24)  // 64 bytes to 16 MB
    ->Unit(benchmark::kMicrosecond);

// =============================================================================
// Pattern at Different Positions
// =============================================================================

static void BM_SIMD_Pattern_Beginning(benchmark::State& state) {
    const size_t text_len = 1 << 20;  // 1 MB
    std::string text = generate_text(text_len);
    std::string pattern = "needle";

    // Pattern at beginning
    text.replace(0, pattern.size(), pattern);

    for (auto _ : state) {
        auto result = kmp::search_pos(text, pattern);
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                           static_cast<int64_t>(text_len));
}

BENCHMARK(BM_SIMD_Pattern_Beginning)->Unit(benchmark::kMicrosecond);

static void BM_SIMD_Pattern_Middle(benchmark::State& state) {
    const size_t text_len = 1 << 20;
    std::string text = generate_text(text_len);
    std::string pattern = "needle";

    text.replace(text_len / 2, pattern.size(), pattern);

    for (auto _ : state) {
        auto result = kmp::search_pos(text, pattern);
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                           static_cast<int64_t>(text_len));
}

BENCHMARK(BM_SIMD_Pattern_Middle)->Unit(benchmark::kMicrosecond);

static void BM_SIMD_Pattern_End(benchmark::State& state) {
    const size_t text_len = 1 << 20;
    std::string text = generate_text(text_len);
    std::string pattern = "needle";

    text.replace(text_len - pattern.size(), pattern.size(), pattern);

    for (auto _ : state) {
        auto result = kmp::search_pos(text, pattern);
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                           static_cast<int64_t>(text_len));
}

BENCHMARK(BM_SIMD_Pattern_End)->Unit(benchmark::kMicrosecond);

static void BM_SIMD_Pattern_Not_Found(benchmark::State& state) {
    const size_t text_len = 1 << 20;
    std::string text = generate_text(text_len);
    std::string pattern = "!!!!!!";  // Won't appear in lowercase text

    for (auto _ : state) {
        auto result = kmp::search_pos(text, pattern);
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                           static_cast<int64_t>(text_len));
}

BENCHMARK(BM_SIMD_Pattern_Not_Found)->Unit(benchmark::kMicrosecond);

// =============================================================================
// Pattern Length Scaling
// =============================================================================

static void BM_SIMD_Pattern_Length(benchmark::State& state) {
    const size_t text_len = 1 << 20;
    const size_t pattern_len = static_cast<size_t>(state.range(0));

    std::string text = generate_text(text_len);
    std::string pattern = generate_text(pattern_len, 12345);

    text.replace(text_len / 2, pattern_len, pattern);

    for (auto _ : state) {
        auto result = kmp::search_pos(text, pattern);
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                           static_cast<int64_t>(text_len));
}

BENCHMARK(BM_SIMD_Pattern_Length)
    ->RangeMultiplier(2)
    ->Range(4, 256)
    ->Unit(benchmark::kMicrosecond);

// =============================================================================
// Memory Throughput
// =============================================================================

static void BM_SIMD_Throughput(benchmark::State& state) {
    const size_t text_len = 1 << 24;  // 16 MB
    std::string text = generate_text(text_len);
    std::string pattern = "x";  // Single char for maximum throughput

    // Don't insert - just search
    for (auto _ : state) {
        auto result = kmp::search_pos(text, pattern);
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                           static_cast<int64_t>(text_len));
    state.counters["GB/s"] = benchmark::Counter(
        static_cast<double>(state.bytes_processed()) / 1e9,
        benchmark::Counter::kIsRate);
}

BENCHMARK(BM_SIMD_Throughput)->Unit(benchmark::kMillisecond);

// =============================================================================
// Multiple Searches
// =============================================================================

static void BM_SIMD_Repeated_Search(benchmark::State& state) {
    const size_t text_len = 1 << 20;
    const size_t num_searches = static_cast<size_t>(state.range(0));

    std::string text = generate_text(text_len);
    std::string pattern = "needle";
    text.replace(text_len / 2, pattern.size(), pattern);

    for (auto _ : state) {
        for (size_t i = 0; i < num_searches; ++i) {
            auto result = kmp::search_pos(text, pattern);
            benchmark::DoNotOptimize(result);
        }
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                           static_cast<int64_t>(text_len) *
                           static_cast<int64_t>(num_searches));
}

BENCHMARK(BM_SIMD_Repeated_Search)
    ->RangeMultiplier(4)
    ->Range(1, 256)
    ->Unit(benchmark::kMicrosecond);
