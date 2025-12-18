/**
 * @file test_stress.cpp
 * @brief Stress tests for KMP library - large scale data handling
 */

#include <gtest/gtest.h>
#include <kmp/kmp.hpp>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <numeric>

using namespace kmp;

class StressTest : public ::testing::Test {
protected:
    std::mt19937 rng{42};  // Fixed seed for reproducibility

    std::string generate_random_string(size_t length, const std::string& alphabet = "abcdefghijklmnopqrstuvwxyz") {
        std::string result;
        result.reserve(length);
        std::uniform_int_distribution<size_t> dist(0, alphabet.size() - 1);
        for (size_t i = 0; i < length; ++i) {
            result += alphabet[dist(rng)];
        }
        return result;
    }

    std::string generate_repeated_string(const std::string& unit, size_t count) {
        std::string result;
        result.reserve(unit.size() * count);
        for (size_t i = 0; i < count; ++i) {
            result += unit;
        }
        return result;
    }
};

// =============================================================================
// Very Large Text Tests (100KB - 10MB)
// =============================================================================

TEST_F(StressTest, SearchIn100KB) {
    std::string text = generate_random_string(100'000);
    std::string pattern = "needle";

    // Insert pattern at random position
    size_t pos = 50'000;
    text.replace(pos, pattern.size(), pattern);

    auto result = search_pos(text, pattern);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, pos);
}

TEST_F(StressTest, SearchIn1MB) {
    std::string text = generate_random_string(1'000'000);
    std::string pattern = "findme123";

    size_t pos = 750'000;
    text.replace(pos, pattern.size(), pattern);

    auto result = search_pos(text, pattern);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, pos);
}

TEST_F(StressTest, SearchIn10MB) {
    std::string text = generate_random_string(10'000'000);
    std::string pattern = "verylongpatternstring";

    size_t pos = 9'000'000;
    text.replace(pos, pattern.size(), pattern);

    auto result = search_pos(text, pattern);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, pos);
}

TEST_F(StressTest, NoMatchIn10MB) {
    // Use alphabet without 'z' to guarantee no match
    std::string text = generate_random_string(10'000'000, "abcdefghijklmnopqrstuvwxy");
    std::string pattern = "zzzzz";

    auto result = search_pos(text, pattern);
    EXPECT_FALSE(result.has_value());
}

// =============================================================================
// Long Pattern Tests
// =============================================================================

TEST_F(StressTest, Pattern1KB) {
    std::string pattern = generate_random_string(1'000);
    std::string text = generate_random_string(100'000);

    size_t pos = 50'000;
    text.replace(pos, pattern.size(), pattern);

    auto result = search_pos(text, pattern);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, pos);
}

TEST_F(StressTest, Pattern10KB) {
    std::string pattern = generate_random_string(10'000);
    std::string text = generate_random_string(1'000'000);

    size_t pos = 500'000;
    text.replace(pos, pattern.size(), pattern);

    auto result = search_pos(text, pattern);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, pos);
}

TEST_F(StressTest, PatternAlmostAsLongAsText) {
    std::string pattern = generate_random_string(9'900);
    std::string text = generate_random_string(10'000);

    // Replace entire middle section
    text.replace(50, pattern.size(), pattern);

    auto result = search_pos(text, pattern);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 50);
}

// =============================================================================
// Multiple Occurrences Tests
// =============================================================================

TEST_F(StressTest, ThousandOccurrences) {
    std::string unit = "XYZ";
    std::string text = generate_repeated_string(unit, 1000);

    auto results = search_all_vec(text, unit);
    EXPECT_EQ(results.size(), 1000);

    // Verify positions
    for (size_t i = 0; i < results.size(); ++i) {
        EXPECT_EQ(results[i], i * unit.size());
    }
}

TEST_F(StressTest, OverlappingOccurrences10000) {
    // "aaa...aaa" with pattern "aa" - should find n-1 matches
    std::string text(10'001, 'a');

    auto count_result = count(text, "aa");
    EXPECT_EQ(count_result, 10'000);
}

TEST_F(StressTest, DenseOccurrences) {
    // Pattern that overlaps heavily
    // "abababab..." with pattern "abab" - overlapping matches at 0, 2, 4, ...
    std::string text = generate_repeated_string("abab", 10'000);

    auto results = search_all_vec(text, "abab");
    // Pattern "abab" in "abababab..." matches at positions 0, 2, 4, ...
    // Text length is 40000, so matches at 0, 2, 4, ..., 39996
    EXPECT_EQ(results.size(), 19'999);  // (40000 - 4) / 2 + 1
}

TEST_F(StressTest, SparseOccurrences) {
    std::string separator(1000, 'x');
    std::string pattern = "MARKER";

    std::string text;
    for (int i = 0; i < 100; ++i) {
        text += pattern + separator;
    }

    auto results = search_all_vec(text, pattern);
    EXPECT_EQ(results.size(), 100);
}

// =============================================================================
// Worst-Case Pattern Tests (for KMP efficiency)
// =============================================================================

TEST_F(StressTest, WorstCasePatternAllSame) {
    // Pattern: "aaaaa", Text: "aaaaaaaaaa...b"
    // This is a classic worst-case for naive algorithms
    std::string text(1'000'000, 'a');
    text += 'b';
    std::string pattern(1000, 'a');
    pattern += 'b';

    auto result = search_pos(text, pattern);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 1'000'000 - 1000);
}

TEST_F(StressTest, WorstCaseRepeatingPrefix) {
    // Pattern with long repeating prefix: "abcabcabcabcd"
    std::string unit = "abc";
    std::string pattern = generate_repeated_string(unit, 333);
    pattern += "d";

    std::string text = generate_repeated_string(unit, 100'000);
    text += "d";

    auto result = search_pos(text, pattern);
    ASSERT_TRUE(result.has_value());
}

TEST_F(StressTest, AlternatingPattern) {
    // "ababab...ababc" pattern - tests failure function heavily
    std::string pattern = generate_repeated_string("ab", 500);
    pattern += "c";

    std::string text = generate_repeated_string("ab", 500'000);
    text.replace(text.size() - pattern.size(), pattern.size(), pattern);

    auto result = search_pos(text, pattern);
    ASSERT_TRUE(result.has_value());
}

// =============================================================================
// Binary Data Tests
// =============================================================================

TEST_F(StressTest, BinaryDataWithNulls) {
    std::string text(10'000, '\0');
    for (size_t i = 0; i < text.size(); i += 100) {
        text[i] = 'X';
    }

    std::string pattern = "\0\0X\0\0";
    pattern.resize(5);  // Ensure correct size with nulls

    // Create the pattern in text
    text.replace(5000, pattern.size(), pattern);

    auto result = search_pos(text, pattern);
    ASSERT_TRUE(result.has_value());
}

TEST_F(StressTest, AllBytesPattern) {
    // Pattern containing all possible byte values
    std::string pattern;
    for (int i = 0; i < 256; ++i) {
        pattern += static_cast<char>(i);
    }

    std::string text = generate_random_string(100'000);
    text.replace(50'000, pattern.size(), pattern);

    auto result = search_pos(text, pattern);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 50'000);
}

// =============================================================================
// Repeated Search Tests (Pattern Reuse)
// =============================================================================

TEST_F(StressTest, RepeatedSearchSamePattern) {
    literal_pattern pattern("findme");

    for (int i = 0; i < 1000; ++i) {
        std::string text = generate_random_string(10'000);
        size_t pos = i * 10 % 9000;
        text.replace(pos, pattern.size(), pattern.pattern());

        auto it = search(text.begin(), text.end(), pattern.begin(), pattern.end());
        EXPECT_NE(it, text.end());
    }
}

TEST_F(StressTest, RepeatedSearchDifferentTexts) {
    std::string pattern = "target";
    std::vector<std::string> texts;

    for (int i = 0; i < 100; ++i) {
        texts.push_back(generate_random_string(100'000));
        texts.back().replace(i * 1000 % 99000, pattern.size(), pattern);
    }

    for (const auto& text : texts) {
        auto result = search_pos(text, pattern);
        ASSERT_TRUE(result.has_value());
    }
}

// =============================================================================
// Memory Stress Tests
// =============================================================================

TEST_F(StressTest, ManySmallSearches) {
    // Perform many small searches to test memory management
    for (int i = 0; i < 10'000; ++i) {
        std::string text = generate_random_string(100 + i % 900);
        std::string pattern = generate_random_string(5 + i % 10);

        // Just ensure it doesn't crash
        auto result = search_pos(text, pattern);
        (void)result;
    }
}

TEST_F(StressTest, SearchAllManyResults) {
    // Search that returns many results
    std::string text = generate_repeated_string("ab", 100'000);

    auto results = search_all_vec(text, "ab");
    EXPECT_EQ(results.size(), 100'000);
}

// =============================================================================
// Boundary Condition Tests
// =============================================================================

TEST_F(StressTest, PatternAtVeryEnd) {
    std::string text = generate_random_string(1'000'000, "abcdefghijklmnopqrstuvwxy");
    std::string pattern = "ZZZZZ";
    text += pattern;

    auto result = search_pos(text, pattern);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 1'000'000);
}

TEST_F(StressTest, PatternAtVeryBeginning) {
    std::string pattern = "START";
    std::string text = pattern + generate_random_string(1'000'000);

    auto result = search_pos(text, pattern);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0);
}

TEST_F(StressTest, MultiplePatternsBoundary) {
    std::string pattern = "MARKER";
    std::string text = pattern + generate_random_string(999'988) + pattern;

    auto results = search_all_vec(text, pattern);
    ASSERT_GE(results.size(), 2);
    EXPECT_EQ(results.front(), 0);
    EXPECT_EQ(results.back(), text.size() - pattern.size());
}
