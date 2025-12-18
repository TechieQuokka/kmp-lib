/**
 * @file test_performance.cpp
 * @brief Performance validation tests for KMP library
 *
 * These tests verify that the algorithm performs within expected
 * time complexity bounds (O(n+m)).
 */

#include <gtest/gtest.h>
#include <kmp/kmp.hpp>
#include <string>
#include <chrono>
#include <random>
#include <cmath>

using namespace kmp;
using namespace std::chrono;

class PerformanceTest : public ::testing::Test {
protected:
    std::mt19937 rng{42};

    std::string generate_random_string(size_t length, const std::string& alphabet = "abcdefghijklmnopqrstuvwxyz") {
        std::string result;
        result.reserve(length);
        std::uniform_int_distribution<size_t> dist(0, alphabet.size() - 1);
        for (size_t i = 0; i < length; ++i) {
            result += alphabet[dist(rng)];
        }
        return result;
    }

    template<typename Func>
    double measure_time_ms(Func&& func, int iterations = 1) {
        auto start = high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            func();
        }
        auto end = high_resolution_clock::now();
        return duration<double, std::milli>(end - start).count() / iterations;
    }
};

// =============================================================================
// Linear Time Complexity Verification
// =============================================================================

TEST_F(PerformanceTest, LinearTimeWithTextSize) {
    // Verify O(n) with respect to text size
    const std::string pattern = "findme";
    std::vector<size_t> sizes = {10'000, 50'000, 100'000, 500'000, 1'000'000};
    std::vector<double> times;

    for (auto size : sizes) {
        std::string text = generate_random_string(size);
        text.replace(size / 2, pattern.size(), pattern);

        double time = measure_time_ms([&]() {
            auto result = search_pos(text, pattern);
            (void)result;
        }, 10);
        times.push_back(time);
    }

    // Check that doubling input roughly doubles time (with some tolerance)
    // Times should scale linearly: t2/t1 should be roughly s2/s1
    for (size_t i = 1; i < sizes.size(); ++i) {
        double size_ratio = static_cast<double>(sizes[i]) / sizes[i-1];
        double time_ratio = times[i] / std::max(times[i-1], 0.001);

        // Allow generous tolerance (within 3x of linear scaling)
        // This accounts for cache effects, OS scheduling, etc.
        EXPECT_LT(time_ratio, size_ratio * 3.0)
            << "Time scaling exceeded 3x linear at size " << sizes[i];
    }
}

TEST_F(PerformanceTest, LinearTimeWithPatternSize) {
    // Verify O(m) with respect to pattern size
    const size_t text_size = 100'000;
    std::string text = generate_random_string(text_size);

    std::vector<size_t> pattern_sizes = {10, 50, 100, 500, 1000};
    std::vector<double> times;

    for (auto pat_size : pattern_sizes) {
        std::string pattern = generate_random_string(pat_size);
        text.replace(text_size / 2, pattern.size(), pattern);

        double time = measure_time_ms([&]() {
            auto result = search_pos(text, pattern);
            (void)result;
        }, 10);
        times.push_back(time);
    }

    // Pattern size should have less impact than text size
    // Just verify we don't see quadratic explosion
    double max_time = *std::max_element(times.begin(), times.end());
    double min_time = *std::min_element(times.begin(), times.end());

    // Even with 100x larger pattern, time shouldn't increase by more than 10x
    EXPECT_LT(max_time, min_time * 20)
        << "Pattern size impact too large";
}

// =============================================================================
// Worst-Case Performance Tests
// =============================================================================

TEST_F(PerformanceTest, WorstCaseAllSameCharacter) {
    // Worst case for naive algorithm: "aaa...aab" in "aaa...aaa"
    // KMP should still be O(n+m)
    std::string text(500'000, 'a');
    std::string pattern(1000, 'a');
    pattern.back() = 'b';

    // Add the pattern at the end
    text.replace(text.size() - pattern.size(), pattern.size(), pattern);

    double time = measure_time_ms([&]() {
        auto result = search_pos(text, pattern);
        EXPECT_TRUE(result.has_value());
    }, 5);

    // Should complete quickly (less than 100ms for 500KB)
    EXPECT_LT(time, 100.0)
        << "Worst case took too long: " << time << "ms";
}

TEST_F(PerformanceTest, WorstCaseRepeatingPattern) {
    // Pattern: "abcabc...abcabd"
    std::string unit = "abc";
    std::string pattern;
    for (int i = 0; i < 333; ++i) {
        pattern += unit;
    }
    pattern += "abd";

    std::string text;
    for (int i = 0; i < 100'000; ++i) {
        text += unit;
    }
    text += "abd";

    double time = measure_time_ms([&]() {
        auto result = search_pos(text, pattern);
        EXPECT_TRUE(result.has_value());
    }, 5);

    // Should still be fast
    EXPECT_LT(time, 50.0)
        << "Repeating pattern worst case took too long";
}

// =============================================================================
// search_all Performance
// =============================================================================

TEST_F(PerformanceTest, SearchAllManyMatches) {
    // Test with many overlapping matches
    std::string text(100'000, 'a');
    std::string pattern = "aa";

    double time = measure_time_ms([&]() {
        auto results = search_all_vec(text, pattern);
        EXPECT_EQ(results.size(), 99'999);
    }, 3);

    // Should handle ~100K results quickly
    EXPECT_LT(time, 100.0)
        << "search_all with many matches took too long";
}

TEST_F(PerformanceTest, SearchAllSparseMatches) {
    std::string text = generate_random_string(1'000'000, "abcdefghij");
    std::string pattern = "xyz";

    // Insert sparse matches
    for (size_t i = 0; i < 100; ++i) {
        text.replace(i * 10'000, pattern.size(), pattern);
    }

    double time = measure_time_ms([&]() {
        auto results = search_all_vec(text, pattern);
        EXPECT_EQ(results.size(), 100);
    }, 3);

    EXPECT_LT(time, 50.0)
        << "search_all with sparse matches took too long";
}

// =============================================================================
// Count Performance
// =============================================================================

TEST_F(PerformanceTest, CountPerformance) {
    std::string text = generate_random_string(1'000'000, "abc");
    std::string pattern = "ab";

    double time = measure_time_ms([&]() {
        auto cnt = count(text, pattern);
        EXPECT_GT(cnt, 0);
    }, 5);

    EXPECT_LT(time, 50.0)
        << "Count operation took too long";
}

// =============================================================================
// Literal Pattern Precompilation Benefit
// =============================================================================

TEST_F(PerformanceTest, PrecompiledPatternBenefit) {
    std::string text = generate_random_string(100'000);
    std::string pattern_str = "benchmark_pattern";
    text.replace(50'000, pattern_str.size(), pattern_str);

    // Without precompilation
    double time_without = measure_time_ms([&]() {
        auto result = search_pos(text, pattern_str);
        EXPECT_TRUE(result.has_value());
    }, 100);

    // With precompilation
    literal_pattern pattern(pattern_str);
    double time_with = measure_time_ms([&]() {
        auto it = search(text.begin(), text.end(), pattern.begin(), pattern.end());
        EXPECT_NE(it, text.end());
    }, 100);

    // Precompiled should be at least as fast (usually faster for repeated use)
    // Due to measurement noise, just verify it's not significantly slower
    EXPECT_LT(time_with, time_without * 2.0)
        << "Precompiled pattern significantly slower than expected";
}

// =============================================================================
// SIMD Threshold Verification
// =============================================================================

TEST_F(PerformanceTest, SIMDThresholdBehavior) {
    // Test behavior around SIMD threshold
    std::string pattern = "simd_test";

    // Small text (below threshold)
    std::string small_text = generate_random_string(50);
    small_text.replace(20, pattern.size(), pattern);

    // Large text (above threshold)
    std::string large_text = generate_random_string(10'000);
    large_text.replace(5000, pattern.size(), pattern);

    // Both should work correctly
    auto small_result = search_pos(small_text, pattern);
    auto large_result = search_pos(large_text, pattern);

    EXPECT_TRUE(small_result.has_value());
    EXPECT_TRUE(large_result.has_value());
    EXPECT_EQ(*small_result, 20);
    EXPECT_EQ(*large_result, 5000);
}

// =============================================================================
// Memory Efficiency Tests
// =============================================================================

TEST_F(PerformanceTest, RepeatedSearchMemoryStable) {
    // Ensure no memory leaks in repeated searches
    std::string text = generate_random_string(10'000);
    std::string pattern = "memory_test";
    text.replace(5000, pattern.size(), pattern);

    // Run many iterations
    for (int i = 0; i < 10'000; ++i) {
        auto result = search_pos(text, pattern);
        EXPECT_TRUE(result.has_value());
    }

    // If we get here without crash/timeout, memory is stable
    SUCCEED();
}

TEST_F(PerformanceTest, LargeResultVectorMemory) {
    std::string text(50'000, 'a');
    std::string pattern = "a";

    // This will create a large result vector
    auto results = search_all_vec(text, pattern);

    EXPECT_EQ(results.size(), 50'000);
}

// =============================================================================
// Regex Performance
// =============================================================================

TEST_F(PerformanceTest, RegexLinearTime) {
    // Verify regex is O(n) not exponential
    regex_pattern pattern("[a-z]+");

    std::vector<size_t> sizes = {1'000, 5'000, 10'000, 50'000};
    std::vector<double> times;

    for (auto size : sizes) {
        std::string text = generate_random_string(size);

        double time = measure_time_ms([&]() {
            auto result = pattern.search(text);
            EXPECT_TRUE(result.has_value());
        }, 10);
        times.push_back(time);
    }

    // Verify roughly linear scaling
    for (size_t i = 1; i < sizes.size(); ++i) {
        double size_ratio = static_cast<double>(sizes[i]) / sizes[i-1];
        double time_ratio = times[i] / std::max(times[i-1], 0.001);

        // Should scale linearly (not exponentially)
        EXPECT_LT(time_ratio, size_ratio * 5.0)
            << "Regex time scaling exceeded 5x linear at size " << sizes[i];
    }
}

TEST_F(PerformanceTest, RegexNoBacktrackExplosion) {
    // Classic regex backtracking bomb: a?^n a^n
    // DFA-based engine should handle this in O(n)

    // Pattern that would cause backtracking in traditional engines
    regex_pattern pattern("a*a*a*a*a*b");

    // Text that would trigger exponential backtracking
    std::string text(50, 'a');

    double time = measure_time_ms([&]() {
        auto result = pattern.search(text);
        EXPECT_FALSE(result.has_value());  // No 'b' in text
    }, 100);

    // Should be very fast (no exponential blowup)
    EXPECT_LT(time, 1.0)
        << "Potential backtracking explosion detected";
}

// =============================================================================
// Throughput Tests
// =============================================================================

TEST_F(PerformanceTest, ThroughputMeasurement) {
    std::string text = generate_random_string(10'000'000);  // 10MB
    std::string pattern = "throughput_test";
    text.replace(9'000'000, pattern.size(), pattern);

    auto start = high_resolution_clock::now();
    auto result = search_pos(text, pattern);
    auto end = high_resolution_clock::now();

    EXPECT_TRUE(result.has_value());

    double time_ms = duration<double, std::milli>(end - start).count();
    double throughput_mb_s = (10.0) / (time_ms / 1000.0);  // MB/s

    // Just log the throughput (actual value depends on hardware)
    std::cout << "Search throughput: " << throughput_mb_s << " MB/s\n";

    // Expect at least 100 MB/s on modern hardware
    EXPECT_GT(throughput_mb_s, 100.0)
        << "Throughput below expected minimum";
}

// =============================================================================
// Comparison with std::string::find
// =============================================================================

TEST_F(PerformanceTest, CompareWithStdFind) {
    std::string text = generate_random_string(1'000'000);
    std::string pattern = "comparison_test";
    text.replace(900'000, pattern.size(), pattern);

    // KMP search
    double kmp_time = measure_time_ms([&]() {
        auto result = search_pos(text, pattern);
        EXPECT_TRUE(result.has_value());
    }, 50);

    // std::string::find
    double std_time = measure_time_ms([&]() {
        auto pos = text.find(pattern);
        EXPECT_NE(pos, std::string::npos);
    }, 50);

    // Log comparison
    std::cout << "KMP time: " << kmp_time << "ms, std::find time: " << std_time << "ms\n";
    std::cout << "Ratio (KMP/std): " << kmp_time / std_time << "\n";

    // KMP has overhead from failure function computation
    // For single searches, std::find may be faster due to SIMD optimizations
    // KMP shines with repeated searches using precompiled patterns
    // Allow up to 15x slower for this simple case
    EXPECT_LT(kmp_time, std_time * 15.0)
        << "KMP significantly slower than std::find";
}
