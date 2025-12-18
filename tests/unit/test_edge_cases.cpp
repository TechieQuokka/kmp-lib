/**
 * @file test_edge_cases.cpp
 * @brief Edge case and boundary condition tests for KMP library
 */

#include <gtest/gtest.h>
#include <kmp/kmp.hpp>
#include <string>
#include <vector>
#include <limits>

using namespace kmp;

class EdgeCaseTest : public ::testing::Test {};

// =============================================================================
// Single Character Tests
// =============================================================================

TEST_F(EdgeCaseTest, SingleCharPatternSingleCharText) {
    EXPECT_TRUE(contains("a", "a"));
    EXPECT_FALSE(contains("a", "b"));
}

TEST_F(EdgeCaseTest, SingleCharPatternInText) {
    auto results = search_all_vec("abcabc", "a");
    ASSERT_EQ(results.size(), 2);
    EXPECT_EQ(results[0], 0);
    EXPECT_EQ(results[1], 3);
}

TEST_F(EdgeCaseTest, SingleCharRepeated) {
    auto results = search_all_vec("aaaaaaa", "a");
    EXPECT_EQ(results.size(), 7);
}

TEST_F(EdgeCaseTest, SingleCharNotFound) {
    EXPECT_FALSE(contains("bcdefg", "a"));
}

// =============================================================================
// Special Characters Tests
// =============================================================================

TEST_F(EdgeCaseTest, SpacesInPattern) {
    auto result = search_pos("hello world test", "o w");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 4);
}

TEST_F(EdgeCaseTest, TabsAndNewlines) {
    std::string text = "line1\tcolumn2\nline2\tcolumn2";
    EXPECT_TRUE(contains(text, "\t"));
    EXPECT_TRUE(contains(text, "\n"));
    EXPECT_TRUE(contains(text, "\tcolumn2\n"));
}

TEST_F(EdgeCaseTest, CarriageReturn) {
    std::string text = "windows\r\nline\r\nending";
    EXPECT_TRUE(contains(text, "\r\n"));

    auto results = search_all_vec(text, "\r\n");
    EXPECT_EQ(results.size(), 2);
}

TEST_F(EdgeCaseTest, NullCharacterInMiddle) {
    std::string text = "before";
    text += '\0';
    text += "after";

    std::string pattern = "e";
    pattern += '\0';
    pattern += "a";

    EXPECT_TRUE(contains(text, pattern));
}

TEST_F(EdgeCaseTest, OnlySpecialChars) {
    std::string text = "\t\n\r \t\n\r ";
    std::string pattern = "\n\r ";

    auto results = search_all_vec(text, pattern);
    EXPECT_EQ(results.size(), 2);
}

// =============================================================================
// Identical Text and Pattern Tests
// =============================================================================

TEST_F(EdgeCaseTest, IdenticalTextAndPattern) {
    std::string str = "exactly the same string";
    auto result = search_pos(str, str);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0);
}

TEST_F(EdgeCaseTest, PatternOneLongerThanText) {
    EXPECT_FALSE(contains("hello", "hello!"));
}

TEST_F(EdgeCaseTest, PatternOneCharShorterThanText) {
    auto result = search_pos("hello", "hell");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0);
}

// =============================================================================
// Repeated Character Patterns
// =============================================================================

TEST_F(EdgeCaseTest, AllSameCharacters) {
    auto results = search_all_vec("aaaa", "aa");
    ASSERT_EQ(results.size(), 3);
    EXPECT_EQ(results[0], 0);
    EXPECT_EQ(results[1], 1);
    EXPECT_EQ(results[2], 2);
}

TEST_F(EdgeCaseTest, AlternatingCharacters) {
    auto results = search_all_vec("ababababab", "abab");
    ASSERT_EQ(results.size(), 4);
}

TEST_F(EdgeCaseTest, PalindromePattern) {
    std::string text = "abcbaXabcba";
    auto results = search_all_vec(text, "abcba");
    ASSERT_EQ(results.size(), 2);
    EXPECT_EQ(results[0], 0);
    EXPECT_EQ(results[1], 6);
}

// =============================================================================
// Prefix/Suffix Overlap Tests
// =============================================================================

TEST_F(EdgeCaseTest, PatternIsPrefixOfText) {
    auto result = search_pos("prefixSUFFIX", "prefix");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0);
}

TEST_F(EdgeCaseTest, PatternIsSuffixOfText) {
    auto result = search_pos("PREFIXsuffix", "suffix");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 6);
}

TEST_F(EdgeCaseTest, PatternWithSelfOverlap) {
    // Pattern "abab" has prefix "ab" = suffix "ab"
    std::string text = "ababab";
    auto results = search_all_vec(text, "abab");
    ASSERT_EQ(results.size(), 2);
    EXPECT_EQ(results[0], 0);
    EXPECT_EQ(results[1], 2);
}

TEST_F(EdgeCaseTest, ComplexSelfOverlap) {
    // "ABCABC" has complex failure function
    std::string text = "ABCABCABCABC";
    auto results = search_all_vec(text, "ABCABC");
    ASSERT_EQ(results.size(), 3);
}

// =============================================================================
// Near-Match Tests
// =============================================================================

TEST_F(EdgeCaseTest, OffByOneCharacter) {
    EXPECT_FALSE(contains("hello world", "hellp"));
    EXPECT_FALSE(contains("hello world", "hallo"));
}

TEST_F(EdgeCaseTest, PartialMatchThenFail) {
    // Text has partial match that fails at the end
    std::string text = "abcabcabd";
    EXPECT_TRUE(contains(text, "abcabc"));
    EXPECT_FALSE(contains(text, "abcabcabc"));
}

TEST_F(EdgeCaseTest, MultiplePartialMatches) {
    // Multiple false starts before real match - test with a pattern that exists
    std::string text = "abcabdabcabc";
    auto result = search_pos(text, "abcabc");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 6);  // Found at position 6
}

// =============================================================================
// Case Sensitivity Tests
// =============================================================================

TEST_F(EdgeCaseTest, CaseSensitiveMatch) {
    EXPECT_TRUE(contains("Hello World", "Hello"));
    EXPECT_FALSE(contains("Hello World", "hello"));
}

TEST_F(EdgeCaseTest, MixedCase) {
    auto results = search_all_vec("AaAaAa", "Aa");
    EXPECT_EQ(results.size(), 3);
}

// =============================================================================
// Numeric String Tests
// =============================================================================

TEST_F(EdgeCaseTest, NumericPatterns) {
    std::string text = "12345678901234567890";
    auto results = search_all_vec(text, "123");
    ASSERT_EQ(results.size(), 2);
    EXPECT_EQ(results[0], 0);
    EXPECT_EQ(results[1], 10);
}

TEST_F(EdgeCaseTest, MixedAlphanumeric) {
    std::string text = "abc123abc123xyz";
    auto results = search_all_vec(text, "abc123");
    EXPECT_EQ(results.size(), 2);
}

// =============================================================================
// Unicode-like Byte Sequences
// =============================================================================

TEST_F(EdgeCaseTest, HighByteValues) {
    std::string text;
    text += static_cast<char>(0xFF);
    text += static_cast<char>(0xFE);
    text += static_cast<char>(0xFD);
    text += "normal";
    text += static_cast<char>(0xFF);
    text += static_cast<char>(0xFE);

    std::string pattern;
    pattern += static_cast<char>(0xFF);
    pattern += static_cast<char>(0xFE);

    auto results = search_all_vec(text, pattern);
    EXPECT_EQ(results.size(), 2);
}

TEST_F(EdgeCaseTest, AllHighBytes) {
    std::string text(100, static_cast<char>(0xFF));
    std::string pattern(5, static_cast<char>(0xFF));

    auto results = search_all_vec(text, pattern);
    EXPECT_EQ(results.size(), 96);  // 100 - 5 + 1
}

// =============================================================================
// Generator/Iterator Tests
// =============================================================================

TEST_F(EdgeCaseTest, SearchAllLazyEvaluation) {
    std::string text(100'000, 'a');
    text[1000] = 'b';
    text += "b";

    // Using generator - should not process entire text if we only need first
    auto gen = search_all(text, "ab");
    auto it = gen.begin();
    EXPECT_EQ(*it, 999);
}

TEST_F(EdgeCaseTest, EmptyGeneratorResult) {
    auto gen = search_all("hello", "xyz");
    EXPECT_EQ(gen.begin(), gen.end());
}

// =============================================================================
// Count Edge Cases
// =============================================================================

TEST_F(EdgeCaseTest, CountZeroMatches) {
    EXPECT_EQ(count("abcdef", "xyz"), 0);
}

TEST_F(EdgeCaseTest, CountOneMatch) {
    EXPECT_EQ(count("abcdef", "cde"), 1);
}

TEST_F(EdgeCaseTest, CountOverlappingMax) {
    // Maximum overlapping for pattern of length 2
    EXPECT_EQ(count("aaaaaaaaaa", "aa"), 9);
}

TEST_F(EdgeCaseTest, CountEmptyPattern) {
    // Empty pattern behavior - returns 0 (no iterations)
    EXPECT_EQ(count("hello", ""), 0);
}

// =============================================================================
// Contains Edge Cases
// =============================================================================

TEST_F(EdgeCaseTest, ContainsEmptyInNonEmpty) {
    EXPECT_TRUE(contains("hello", ""));
}

TEST_F(EdgeCaseTest, ContainsInEmpty) {
    EXPECT_FALSE(contains("", "a"));
}

TEST_F(EdgeCaseTest, ContainsBothEmpty) {
    // Empty pattern in empty text
    // For empty text with empty pattern, iterator-based search returns text.begin() == text.end()
    // This means search_pos returns nullopt (since result == text.end())
    std::string_view empty_text = "";
    std::string_view empty_pattern = "";
    auto it = search(empty_text, empty_pattern);
    // When both are empty, search returns text.begin() which equals text.end()
    EXPECT_EQ(it, empty_text.begin());
}

// =============================================================================
// Position at Boundaries
// =============================================================================

TEST_F(EdgeCaseTest, MatchAtPosition0) {
    auto result = search_pos("pattern at start", "pattern");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0);
}

TEST_F(EdgeCaseTest, MatchAtLastPossiblePosition) {
    auto result = search_pos("text with pattern", "pattern");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 10);
}

TEST_F(EdgeCaseTest, MatchExactlyFitsAtEnd) {
    std::string text = "prefixEND";
    auto result = search_pos(text, "END");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 6);
}

// =============================================================================
// Pathological Patterns
// =============================================================================

TEST_F(EdgeCaseTest, MaximumFailureFunctionValues) {
    // Pattern where failure function has maximum possible values
    // "aaaa" -> failure = [0, 1, 2, 3]
    literal_pattern pat("aaaaa");
    auto& failure = pat.failure();

    EXPECT_EQ(failure[0], 0);
    EXPECT_EQ(failure[1], 1);
    EXPECT_EQ(failure[2], 2);
    EXPECT_EQ(failure[3], 3);
    EXPECT_EQ(failure[4], 4);
}

TEST_F(EdgeCaseTest, ZeroFailureFunctionValues) {
    // Pattern with all zeros in failure function
    // "abcdefg" -> failure = [0, 0, 0, 0, 0, 0, 0]
    literal_pattern pat("abcdefg");
    auto& failure = pat.failure();

    for (size_t i = 0; i < pat.size(); ++i) {
        EXPECT_EQ(failure[i], 0);
    }
}

TEST_F(EdgeCaseTest, ClassicKMPFailurePattern) {
    // "ABABAC" -> failure = [0, 0, 1, 2, 3, 0]
    literal_pattern pat("ABABAC");
    auto& failure = pat.failure();

    EXPECT_EQ(failure[0], 0);
    EXPECT_EQ(failure[1], 0);
    EXPECT_EQ(failure[2], 1);
    EXPECT_EQ(failure[3], 2);
    EXPECT_EQ(failure[4], 3);
    EXPECT_EQ(failure[5], 0);
}
