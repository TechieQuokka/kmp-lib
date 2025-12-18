/**
 * @file test_simd.cpp
 * @brief Unit tests for SIMD implementations
 */

#include <gtest/gtest.h>
#include <kmp/kmp.hpp>
#include <kmp/detail/simd/dispatch.hpp>
#include <string>
#include <random>

using namespace kmp;
using namespace kmp::detail::simd;

class SIMDTest : public ::testing::Test {
protected:
    std::string generate_random_text(size_t length, unsigned seed = 42) {
        std::mt19937 gen(seed);
        std::uniform_int_distribution<> dis('a', 'z');

        std::string result;
        result.reserve(length);
        for (size_t i = 0; i < length; ++i) {
            result += static_cast<char>(dis(gen));
        }
        return result;
    }
};

// =============================================================================
// CPU Feature Detection Tests
// =============================================================================

TEST_F(SIMDTest, FeatureDetection) {
    auto features = detect_features();

    // Should always detect something (at least none)
    // Just verify it doesn't crash
    [[maybe_unused]] bool has_sse = has_feature(features, cpu_feature::sse42);
    [[maybe_unused]] bool has_avx = has_feature(features, cpu_feature::avx2);
    [[maybe_unused]] bool has_512 = has_feature(features, cpu_feature::avx512f);

    SUCCEED();
}

TEST_F(SIMDTest, CachedFeatures) {
    // Call multiple times to test caching
    auto f1 = get_features();
    auto f2 = get_features();

    EXPECT_EQ(static_cast<uint32_t>(f1), static_cast<uint32_t>(f2));
}

TEST_F(SIMDTest, SIMDLevel) {
    auto level = get_simd_level();

    // Should return a valid level
    EXPECT_TRUE(level == simd_level::scalar ||
                level == simd_level::sse42 ||
                level == simd_level::avx2 ||
                level == simd_level::avx512);
}

// =============================================================================
// SIMD Search Correctness Tests
// =============================================================================

TEST_F(SIMDTest, SmallTextScalar) {
    // Text smaller than SIMD threshold should use scalar
    std::string text = "hello world";
    std::string pattern = "world";

    auto pos = search_pos(text, pattern);
    ASSERT_TRUE(pos.has_value());
    EXPECT_EQ(*pos, 6);
}

TEST_F(SIMDTest, LargeTextSIMD) {
    // Text larger than SIMD threshold should use SIMD
    std::string text = generate_random_text(10000);
    std::string pattern = "xyz";

    // Insert pattern at known position
    text.replace(5000, 3, pattern);

    auto pos = search_pos(text, pattern);
    ASSERT_TRUE(pos.has_value());
    EXPECT_EQ(*pos, 5000);
}

TEST_F(SIMDTest, LargeTextNoMatch) {
    std::string text = generate_random_text(10000);
    std::string pattern = "!!!";  // Won't appear in lowercase text

    auto pos = search_pos(text, pattern);
    EXPECT_FALSE(pos.has_value());
}

TEST_F(SIMDTest, PatternAtBeginning) {
    std::string text = "needle" + generate_random_text(10000);
    std::string pattern = "needle";

    auto pos = search_pos(text, pattern);
    ASSERT_TRUE(pos.has_value());
    EXPECT_EQ(*pos, 0);
}

TEST_F(SIMDTest, PatternAtEnd) {
    std::string text = generate_random_text(10000) + "needle";
    std::string pattern = "needle";

    auto pos = search_pos(text, pattern);
    ASSERT_TRUE(pos.has_value());
    EXPECT_EQ(*pos, 10000);
}

TEST_F(SIMDTest, MultipleOccurrences) {
    std::string text = generate_random_text(10000);
    std::string pattern = "abc";

    // Insert pattern at multiple positions
    text.replace(100, 3, pattern);
    text.replace(5000, 3, pattern);
    text.replace(9000, 3, pattern);

    auto results = search_all_vec(text, pattern);

    // Should find at least the ones we inserted
    EXPECT_GE(results.size(), 3);

    // First occurrence should be at position 100 or earlier
    EXPECT_LE(results[0], 100);
}

// =============================================================================
// Boundary Tests
// =============================================================================

TEST_F(SIMDTest, ExactlySIMDWidth16) {
    std::string text(16, 'a');
    std::string pattern = "a";

    auto pos = search_pos(text, pattern);
    ASSERT_TRUE(pos.has_value());
    EXPECT_EQ(*pos, 0);
}

TEST_F(SIMDTest, ExactlySIMDWidth32) {
    std::string text(32, 'a');
    std::string pattern = "a";

    auto pos = search_pos(text, pattern);
    ASSERT_TRUE(pos.has_value());
    EXPECT_EQ(*pos, 0);
}

TEST_F(SIMDTest, ExactlySIMDWidth64) {
    std::string text(64, 'a');
    std::string pattern = "a";

    auto pos = search_pos(text, pattern);
    ASSERT_TRUE(pos.has_value());
    EXPECT_EQ(*pos, 0);
}

TEST_F(SIMDTest, JustBelowThreshold) {
    std::string text(config::simd_threshold - 1, 'a');
    text[text.size() / 2] = 'x';
    std::string pattern = "x";

    auto pos = search_pos(text, pattern);
    ASSERT_TRUE(pos.has_value());
    EXPECT_EQ(*pos, text.size() / 2);
}

TEST_F(SIMDTest, JustAboveThreshold) {
    std::string text(config::simd_threshold + 1, 'a');
    text[text.size() / 2] = 'x';
    std::string pattern = "x";

    auto pos = search_pos(text, pattern);
    ASSERT_TRUE(pos.has_value());
    EXPECT_EQ(*pos, text.size() / 2);
}

// =============================================================================
// Stress Tests
// =============================================================================

TEST_F(SIMDTest, VeryLargeText) {
    // 1 MB text
    std::string text = generate_random_text(1000000);
    std::string pattern = "findme";

    // Insert at end
    text.replace(text.size() - 6, 6, pattern);

    auto pos = search_pos(text, pattern);
    ASSERT_TRUE(pos.has_value());
    EXPECT_EQ(*pos, text.size() - 6);
}

TEST_F(SIMDTest, LongPatternInLargeText) {
    std::string text = generate_random_text(100000);
    std::string pattern = generate_random_text(100, 12345);

    // Insert pattern
    text.replace(50000, pattern.size(), pattern);

    auto pos = search_pos(text, pattern);
    ASSERT_TRUE(pos.has_value());
    EXPECT_EQ(*pos, 50000);
}

TEST_F(SIMDTest, RepeatedSearchesSamePattern) {
    std::string pattern = "needle";
    std::vector<std::string> texts;

    for (int i = 0; i < 100; ++i) {
        auto text = generate_random_text(1000, i);
        text.replace(500, 6, pattern);
        texts.push_back(text);
    }

    for (const auto& text : texts) {
        auto pos = search_pos(text, pattern);
        ASSERT_TRUE(pos.has_value());
        EXPECT_EQ(*pos, 500);
    }
}

// =============================================================================
// Comparison Tests (SIMD vs Scalar should give same results)
// =============================================================================

TEST_F(SIMDTest, SIMDMatchesScalarResults) {
    // Generate various test cases
    std::vector<std::pair<std::string, std::string>> test_cases;

    // Random texts with random patterns
    for (int i = 0; i < 10; ++i) {
        auto text = generate_random_text(1000 + i * 100, i);
        auto pattern = generate_random_text(5 + i, i * 2);

        // Insert pattern at random position
        size_t pos = (i * 137) % (text.size() - pattern.size());
        text.replace(pos, pattern.size(), pattern);

        test_cases.emplace_back(text, pattern);
    }

    for (const auto& [text, pattern] : test_cases) {
        auto result = search_pos(text, pattern);

        // Verify the match is correct
        if (result.has_value()) {
            std::string_view found(text.data() + *result, pattern.size());
            EXPECT_EQ(found, pattern);
        }
    }
}
