/**
 * @file bench_regex.cpp
 * @brief Benchmarks for DFA regex engine
 */

#include <benchmark/benchmark.h>
#include <kmp/kmp.hpp>
#include <string>
#include <random>
#include <regex>

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

std::string generate_email_like_text(size_t num_emails, unsigned seed = 42) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> len_dis(5, 15);
    std::uniform_int_distribution<> char_dis('a', 'z');

    std::string result;
    for (size_t i = 0; i < num_emails; ++i) {
        // Random text
        size_t text_len = len_dis(gen) * 10;
        for (size_t j = 0; j < text_len; ++j) {
            result += static_cast<char>(char_dis(gen));
        }

        // Email
        size_t user_len = len_dis(gen);
        for (size_t j = 0; j < user_len; ++j) {
            result += static_cast<char>(char_dis(gen));
        }
        result += '@';
        size_t domain_len = len_dis(gen);
        for (size_t j = 0; j < domain_len; ++j) {
            result += static_cast<char>(char_dis(gen));
        }
        result += ".com ";
    }
    return result;
}

} // namespace

// =============================================================================
// DFA Compilation Benchmarks
// =============================================================================

static void BM_Regex_Compile_Simple(benchmark::State& state) {
    for (auto _ : state) {
        auto regex = kmp::compile_regex("hello");
        benchmark::DoNotOptimize(regex);
    }
}

BENCHMARK(BM_Regex_Compile_Simple);

static void BM_Regex_Compile_CharClass(benchmark::State& state) {
    for (auto _ : state) {
        auto regex = kmp::compile_regex("[a-zA-Z0-9]+");
        benchmark::DoNotOptimize(regex);
    }
}

BENCHMARK(BM_Regex_Compile_CharClass);

static void BM_Regex_Compile_Complex(benchmark::State& state) {
    for (auto _ : state) {
        auto regex = kmp::compile_regex("[a-z]+@[a-z]+\\.[a-z]+");
        benchmark::DoNotOptimize(regex);
    }
}

BENCHMARK(BM_Regex_Compile_Complex);

static void BM_Regex_Compile_Alternation(benchmark::State& state) {
    for (auto _ : state) {
        auto regex = kmp::compile_regex("(cat|dog|bird|fish)+");
        benchmark::DoNotOptimize(regex);
    }
}

BENCHMARK(BM_Regex_Compile_Alternation);

// =============================================================================
// DFA Matching Benchmarks
// =============================================================================

static void BM_Regex_Match_Simple(benchmark::State& state) {
    auto regex = kmp::compile_regex("hello");
    std::string text = "hello world";

    for (auto _ : state) {
        auto result = regex.search(text);
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(BM_Regex_Match_Simple);

static void BM_Regex_Match_CharClass(benchmark::State& state) {
    auto regex = kmp::compile_regex("[a-z]+");
    std::string text = generate_text(10000);

    for (auto _ : state) {
        auto result = regex.search(text);
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * 10000);
}

BENCHMARK(BM_Regex_Match_CharClass)->Unit(benchmark::kMicrosecond);

static void BM_Regex_Match_Email(benchmark::State& state) {
    auto regex = kmp::compile_regex("[a-z]+@[a-z]+\\.[a-z]+");
    std::string text = generate_email_like_text(100);

    for (auto _ : state) {
        auto result = regex.search(text);
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                           static_cast<int64_t>(text.size()));
}

BENCHMARK(BM_Regex_Match_Email)->Unit(benchmark::kMicrosecond);

// =============================================================================
// Scaling Benchmarks
// =============================================================================

static void BM_Regex_Scaling(benchmark::State& state) {
    const size_t text_len = static_cast<size_t>(state.range(0));
    auto regex = kmp::compile_regex("[0-9]+");
    std::string text = generate_text(text_len);

    // Insert some digits
    text.replace(text_len / 2, 5, "12345");

    for (auto _ : state) {
        auto result = regex.search(text);
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                           static_cast<int64_t>(text_len));
}

BENCHMARK(BM_Regex_Scaling)
    ->RangeMultiplier(4)
    ->Range(256, 1 << 20)
    ->Unit(benchmark::kMicrosecond);

// =============================================================================
// Comparison with std::regex (for reference)
// =============================================================================

static void BM_STD_Regex_Compile(benchmark::State& state) {
    for (auto _ : state) {
        std::regex regex("[a-z]+@[a-z]+\\.[a-z]+");
        benchmark::DoNotOptimize(regex);
    }
}

BENCHMARK(BM_STD_Regex_Compile);

static void BM_STD_Regex_Match(benchmark::State& state) {
    std::regex regex("[a-z]+@[a-z]+\\.[a-z]+");
    std::string text = generate_email_like_text(100);

    for (auto _ : state) {
        std::smatch match;
        bool result = std::regex_search(text, match, regex);
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                           static_cast<int64_t>(text.size()));
}

BENCHMARK(BM_STD_Regex_Match)->Unit(benchmark::kMicrosecond);

// =============================================================================
// DFA State Count Impact
// =============================================================================

static void BM_Regex_DFA_States(benchmark::State& state) {
    // Pattern complexity affects DFA size
    std::string pattern;
    const int complexity = static_cast<int>(state.range(0));

    for (int i = 0; i < complexity; ++i) {
        if (i > 0) pattern += "|";
        pattern += std::string(1, static_cast<char>('a' + i % 26));
        pattern += "+";
    }

    auto regex = kmp::compile_regex(pattern);
    std::string text = generate_text(10000);

    state.counters["DFA_States"] = static_cast<double>(regex.state_count());

    for (auto _ : state) {
        auto result = regex.search(text);
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * 10000);
}

BENCHMARK(BM_Regex_DFA_States)
    ->RangeMultiplier(2)
    ->Range(1, 16)
    ->Unit(benchmark::kMicrosecond);

// =============================================================================
// Full Match vs Search
// =============================================================================

static void BM_Regex_Full_Match(benchmark::State& state) {
    auto regex = kmp::compile_regex("[a-z]+");
    std::string text = generate_text(1000);

    for (auto _ : state) {
        bool result = regex.matches(text);
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * 1000);
}

BENCHMARK(BM_Regex_Full_Match);

static void BM_Regex_Partial_Search(benchmark::State& state) {
    auto regex = kmp::compile_regex("[0-9]+");
    std::string text = generate_text(1000);
    text.replace(500, 5, "12345");

    for (auto _ : state) {
        auto result = regex.search(text);
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * 1000);
}

BENCHMARK(BM_Regex_Partial_Search);
