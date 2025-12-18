/**
 * @file test_search.cpp
 * @brief Unit tests for KMP search functions
 */

#include <gtest/gtest.h>
#include <kmp/kmp.hpp>
#include <string>
#include <vector>

using namespace kmp;

class SearchTest : public ::testing::Test {};

// =============================================================================
// Basic Search Tests
// =============================================================================

TEST_F(SearchTest, EmptyPattern) {
    std::string text = "hello";
    std::string pattern = "";

    auto it = search(text.begin(), text.end(), pattern.begin(), pattern.end());
    EXPECT_EQ(it, text.begin());
}

TEST_F(SearchTest, EmptyText) {
    std::string text = "";
    std::string pattern = "hello";

    auto it = search(text.begin(), text.end(), pattern.begin(), pattern.end());
    EXPECT_EQ(it, text.end());
}

TEST_F(SearchTest, BothEmpty) {
    std::string text = "";
    std::string pattern = "";

    auto it = search(text.begin(), text.end(), pattern.begin(), pattern.end());
    EXPECT_EQ(it, text.begin());
}

TEST_F(SearchTest, PatternLongerThanText) {
    std::string text = "hi";
    std::string pattern = "hello";

    auto it = search(text.begin(), text.end(), pattern.begin(), pattern.end());
    EXPECT_EQ(it, text.end());
}

TEST_F(SearchTest, ExactMatch) {
    std::string text = "hello";
    std::string pattern = "hello";

    auto it = search(text.begin(), text.end(), pattern.begin(), pattern.end());
    EXPECT_EQ(it, text.begin());
}

TEST_F(SearchTest, MatchAtBeginning) {
    std::string text = "hello world";
    std::string pattern = "hello";

    auto it = search(text.begin(), text.end(), pattern.begin(), pattern.end());
    EXPECT_EQ(it, text.begin());
}

TEST_F(SearchTest, MatchAtEnd) {
    std::string text = "hello world";
    std::string pattern = "world";

    auto it = search(text.begin(), text.end(), pattern.begin(), pattern.end());
    EXPECT_EQ(std::distance(text.begin(), it), 6);
}

TEST_F(SearchTest, MatchInMiddle) {
    std::string text = "hello beautiful world";
    std::string pattern = "beautiful";

    auto it = search(text.begin(), text.end(), pattern.begin(), pattern.end());
    EXPECT_EQ(std::distance(text.begin(), it), 6);
}

TEST_F(SearchTest, NoMatch) {
    std::string text = "hello world";
    std::string pattern = "xyz";

    auto it = search(text.begin(), text.end(), pattern.begin(), pattern.end());
    EXPECT_EQ(it, text.end());
}

TEST_F(SearchTest, PartialMatchNoFull) {
    std::string text = "abcabcabd";
    std::string pattern = "abcabc";

    auto it = search(text.begin(), text.end(), pattern.begin(), pattern.end());
    EXPECT_EQ(it, text.begin());
}

// =============================================================================
// Overlapping Pattern Tests
// =============================================================================

TEST_F(SearchTest, OverlappingPatternReturnsFirst) {
    std::string text = "aaaa";
    std::string pattern = "aa";

    auto it = search(text.begin(), text.end(), pattern.begin(), pattern.end());
    EXPECT_EQ(it, text.begin());
}

TEST_F(SearchTest, ClassicKMPCase) {
    std::string text = "ABABDABACDABABCABAB";
    std::string pattern = "ABABCABAB";

    auto it = search(text.begin(), text.end(), pattern.begin(), pattern.end());
    EXPECT_EQ(std::distance(text.begin(), it), 10);
}

// =============================================================================
// String View Tests
// =============================================================================

TEST_F(SearchTest, StringViewOverload) {
    auto it = search(std::string_view("hello world"),
                     std::string_view("world"));
    EXPECT_NE(it, std::string_view("hello world").end());
}

TEST_F(SearchTest, SearchPosFound) {
    auto pos = search_pos("hello world", "world");
    ASSERT_TRUE(pos.has_value());
    EXPECT_EQ(*pos, 6);
}

TEST_F(SearchTest, SearchPosNotFound) {
    auto pos = search_pos("hello world", "xyz");
    EXPECT_FALSE(pos.has_value());
}

// =============================================================================
// Search All Tests
// =============================================================================

TEST_F(SearchTest, SearchAllNoMatches) {
    auto results = search_all_vec("hello", "xyz");
    EXPECT_TRUE(results.empty());
}

TEST_F(SearchTest, SearchAllSingleMatch) {
    auto results = search_all_vec("hello world", "world");
    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0], 6);
}

TEST_F(SearchTest, SearchAllMultipleNonOverlapping) {
    auto results = search_all_vec("cat dog cat bird cat", "cat");
    ASSERT_EQ(results.size(), 3);
    EXPECT_EQ(results[0], 0);
    EXPECT_EQ(results[1], 8);
    EXPECT_EQ(results[2], 17);
}

TEST_F(SearchTest, SearchAllOverlapping) {
    auto results = search_all_vec("aaaa", "aa");
    ASSERT_EQ(results.size(), 3);
    EXPECT_EQ(results[0], 0);
    EXPECT_EQ(results[1], 1);
    EXPECT_EQ(results[2], 2);
}

TEST_F(SearchTest, SearchAllClassicOverlap) {
    auto results = search_all_vec("ababa", "aba");
    ASSERT_EQ(results.size(), 2);
    EXPECT_EQ(results[0], 0);
    EXPECT_EQ(results[1], 2);
}

// =============================================================================
// Count Tests
// =============================================================================

TEST_F(SearchTest, CountZero) {
    EXPECT_EQ(count("hello", "xyz"), 0);
}

TEST_F(SearchTest, CountMultiple) {
    EXPECT_EQ(count("abababab", "ab"), 4);
}

TEST_F(SearchTest, CountOverlapping) {
    EXPECT_EQ(count("aaaa", "aa"), 3);
}

// =============================================================================
// Contains Tests
// =============================================================================

TEST_F(SearchTest, ContainsTrue) {
    EXPECT_TRUE(contains("hello world", "world"));
}

TEST_F(SearchTest, ContainsFalse) {
    EXPECT_FALSE(contains("hello world", "xyz"));
}

TEST_F(SearchTest, ContainsEmpty) {
    EXPECT_TRUE(contains("hello", ""));
}

// =============================================================================
// Large Text Tests
// =============================================================================

TEST_F(SearchTest, LargeTextSearch) {
    // Create a large text with pattern at the end
    std::string text(10000, 'a');
    text += "needle";

    auto pos = search_pos(text, "needle");
    ASSERT_TRUE(pos.has_value());
    EXPECT_EQ(*pos, 10000);
}

TEST_F(SearchTest, LargeTextNoMatch) {
    std::string text(10000, 'a');

    auto pos = search_pos(text, "needle");
    EXPECT_FALSE(pos.has_value());
}

TEST_F(SearchTest, LargePatternMatch) {
    // Test with distinct pattern characters to avoid edge cases
    std::string prefix = "prefix";  // 6 characters
    std::string pattern;
    for (int i = 0; i < 100; ++i) {
        pattern += "abcdefghij";  // 1000 chars total
    }
    std::string text = prefix + pattern + "suffix";

    auto pos = search_pos(text, pattern);
    ASSERT_TRUE(pos.has_value());
    EXPECT_EQ(*pos, prefix.size());  // Should be 6
}
