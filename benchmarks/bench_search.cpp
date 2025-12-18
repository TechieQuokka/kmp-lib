/**
 * @file bench_search.cpp
 * @brief Benchmarks for KMP search vs std::search
 */

#include <benchmark/benchmark.h>
#include <kmp/kmp.hpp>
#include <algorithm>
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
// KMP Search Benchmarks
// =============================================================================

static void BM_KMP_Search_Short(benchmark::State& state) {
    const size_t text_len = static_cast<size_t>(state.range(0));
    std::string text = generate_text(text_len);
    std::string pattern = "needle";

    // Insert pattern at middle
    text.replace(text_len / 2, pattern.size(), pattern);

    for (auto _ : state) {
        auto result = kmp::search_pos(text, pattern);
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                           static_cast<int64_t>(text_len));
}

BENCHMARK(BM_KMP_Search_Short)
    ->RangeMultiplier(4)
    ->Range(256, 1 << 20)
    ->Unit(benchmark::kMicrosecond);

static void BM_KMP_Search_Long_Pattern(benchmark::State& state) {
    const size_t text_len = static_cast<size_t>(state.range(0));
    std::string text = generate_text(text_len);
    std::string pattern = generate_text(100, 12345);

    text.replace(text_len / 2, pattern.size(), pattern);

    for (auto _ : state) {
        auto result = kmp::search_pos(text, pattern);
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                           static_cast<int64_t>(text_len));
}

BENCHMARK(BM_KMP_Search_Long_Pattern)
    ->RangeMultiplier(4)
    ->Range(1024, 1 << 20)
    ->Unit(benchmark::kMicrosecond);

// =============================================================================
// std::search Comparison
// =============================================================================

static void BM_STD_Search_Short(benchmark::State& state) {
    const size_t text_len = static_cast<size_t>(state.range(0));
    std::string text = generate_text(text_len);
    std::string pattern = "needle";

    text.replace(text_len / 2, pattern.size(), pattern);

    for (auto _ : state) {
        auto result = std::search(text.begin(), text.end(),
                                  pattern.begin(), pattern.end());
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                           static_cast<int64_t>(text_len));
}

BENCHMARK(BM_STD_Search_Short)
    ->RangeMultiplier(4)
    ->Range(256, 1 << 20)
    ->Unit(benchmark::kMicrosecond);

static void BM_STD_Search_Long_Pattern(benchmark::State& state) {
    const size_t text_len = static_cast<size_t>(state.range(0));
    std::string text = generate_text(text_len);
    std::string pattern = generate_text(100, 12345);

    text.replace(text_len / 2, pattern.size(), pattern);

    for (auto _ : state) {
        auto result = std::search(text.begin(), text.end(),
                                  pattern.begin(), pattern.end());
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                           static_cast<int64_t>(text_len));
}

BENCHMARK(BM_STD_Search_Long_Pattern)
    ->RangeMultiplier(4)
    ->Range(1024, 1 << 20)
    ->Unit(benchmark::kMicrosecond);

// =============================================================================
// Search All Benchmarks
// =============================================================================

static void BM_KMP_Search_All(benchmark::State& state) {
    const size_t text_len = static_cast<size_t>(state.range(0));
    std::string text = generate_text(text_len);
    std::string pattern = "ab";

    for (auto _ : state) {
        auto results = kmp::search_all_vec(text, pattern);
        benchmark::DoNotOptimize(results);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                           static_cast<int64_t>(text_len));
}

BENCHMARK(BM_KMP_Search_All)
    ->RangeMultiplier(4)
    ->Range(1024, 1 << 20)
    ->Unit(benchmark::kMicrosecond);

// =============================================================================
// Pre-compiled Pattern Benchmarks
// =============================================================================

static void BM_KMP_Precompiled_Pattern(benchmark::State& state) {
    const size_t text_len = static_cast<size_t>(state.range(0));
    std::string text = generate_text(text_len);

    auto pattern = kmp::compile_literal("needle");
    text.replace(text_len / 2, 6, "needle");

    for (auto _ : state) {
        auto it = kmp::search(text.begin(), text.end(), pattern);
        benchmark::DoNotOptimize(it);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                           static_cast<int64_t>(text_len));
}

BENCHMARK(BM_KMP_Precompiled_Pattern)
    ->RangeMultiplier(4)
    ->Range(1024, 1 << 20)
    ->Unit(benchmark::kMicrosecond);

// =============================================================================
// Worst Case Benchmarks
// =============================================================================

static void BM_KMP_Worst_Case(benchmark::State& state) {
    // Worst case for naive search: aaaaaa...aaaab
    const size_t text_len = static_cast<size_t>(state.range(0));
    std::string text(text_len, 'a');
    text += 'b';
    std::string pattern(text_len / 10, 'a');
    pattern += 'b';

    for (auto _ : state) {
        auto result = kmp::search_pos(text, pattern);
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                           static_cast<int64_t>(text_len));
}

BENCHMARK(BM_KMP_Worst_Case)
    ->RangeMultiplier(4)
    ->Range(1024, 1 << 18)
    ->Unit(benchmark::kMicrosecond);

static void BM_STD_Worst_Case(benchmark::State& state) {
    const size_t text_len = static_cast<size_t>(state.range(0));
    std::string text(text_len, 'a');
    text += 'b';
    std::string pattern(text_len / 10, 'a');
    pattern += 'b';

    for (auto _ : state) {
        auto result = std::search(text.begin(), text.end(),
                                  pattern.begin(), pattern.end());
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                           static_cast<int64_t>(text_len));
}

BENCHMARK(BM_STD_Worst_Case)
    ->RangeMultiplier(4)
    ->Range(1024, 1 << 18)
    ->Unit(benchmark::kMicrosecond);
