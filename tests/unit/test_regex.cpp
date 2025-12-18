/**
 * @file test_regex.cpp
 * @brief Unit tests for DFA regex engine
 */

#include <gtest/gtest.h>
#include <kmp/kmp.hpp>
#include <string>

using namespace kmp;

class RegexTest : public ::testing::Test {};

// =============================================================================
// Basic Literal Tests
// =============================================================================

TEST_F(RegexTest, LiteralMatch) {
    auto regex = compile_regex("hello");

    EXPECT_TRUE(regex.matches("hello"));
    EXPECT_FALSE(regex.matches("world"));
    EXPECT_FALSE(regex.matches("hell"));
    EXPECT_FALSE(regex.matches("helloo"));
}

TEST_F(RegexTest, LiteralSearch) {
    auto regex = compile_regex("world");

    auto pos = regex.search("hello world");
    ASSERT_TRUE(pos.has_value());
    EXPECT_EQ(*pos, 6);
}

TEST_F(RegexTest, LiteralNotFound) {
    auto regex = compile_regex("xyz");

    auto pos = regex.search("hello world");
    EXPECT_FALSE(pos.has_value());
}

// =============================================================================
// Character Class Tests
// =============================================================================

TEST_F(RegexTest, DotMatchesAny) {
    auto regex = compile_regex("h.llo");

    EXPECT_TRUE(regex.matches("hello"));
    EXPECT_TRUE(regex.matches("hallo"));
    EXPECT_TRUE(regex.matches("h9llo"));
    EXPECT_FALSE(regex.matches("hllo"));
}

TEST_F(RegexTest, CharacterClass) {
    auto regex = compile_regex("[abc]");

    EXPECT_TRUE(regex.matches("a"));
    EXPECT_TRUE(regex.matches("b"));
    EXPECT_TRUE(regex.matches("c"));
    EXPECT_FALSE(regex.matches("d"));
}

TEST_F(RegexTest, CharacterRange) {
    auto regex = compile_regex("[a-z]");

    EXPECT_TRUE(regex.matches("a"));
    EXPECT_TRUE(regex.matches("m"));
    EXPECT_TRUE(regex.matches("z"));
    EXPECT_FALSE(regex.matches("A"));
    EXPECT_FALSE(regex.matches("0"));
}

TEST_F(RegexTest, NegatedCharacterClass) {
    auto regex = compile_regex("[^0-9]");

    EXPECT_TRUE(regex.matches("a"));
    EXPECT_TRUE(regex.matches("Z"));
    EXPECT_FALSE(regex.matches("0"));
    EXPECT_FALSE(regex.matches("9"));
}

TEST_F(RegexTest, MultipleRanges) {
    auto regex = compile_regex("[a-zA-Z0-9]");

    EXPECT_TRUE(regex.matches("a"));
    EXPECT_TRUE(regex.matches("Z"));
    EXPECT_TRUE(regex.matches("5"));
    EXPECT_FALSE(regex.matches("!"));
}

// =============================================================================
// Shorthand Classes Tests
// =============================================================================

TEST_F(RegexTest, DigitClass) {
    auto regex = compile_regex("\\d");

    EXPECT_TRUE(regex.matches("0"));
    EXPECT_TRUE(regex.matches("9"));
    EXPECT_FALSE(regex.matches("a"));
}

TEST_F(RegexTest, NonDigitClass) {
    auto regex = compile_regex("\\D");

    EXPECT_TRUE(regex.matches("a"));
    EXPECT_FALSE(regex.matches("0"));
}

TEST_F(RegexTest, WordClass) {
    auto regex = compile_regex("\\w");

    EXPECT_TRUE(regex.matches("a"));
    EXPECT_TRUE(regex.matches("Z"));
    EXPECT_TRUE(regex.matches("0"));
    EXPECT_TRUE(regex.matches("_"));
    EXPECT_FALSE(regex.matches("!"));
}

TEST_F(RegexTest, SpaceClass) {
    auto regex = compile_regex("\\s");

    EXPECT_TRUE(regex.matches(" "));
    EXPECT_TRUE(regex.matches("\t"));
    EXPECT_TRUE(regex.matches("\n"));
    EXPECT_FALSE(regex.matches("a"));
}

// =============================================================================
// Quantifier Tests
// =============================================================================

TEST_F(RegexTest, StarQuantifier) {
    auto regex = compile_regex("ab*c");

    EXPECT_TRUE(regex.matches("ac"));
    EXPECT_TRUE(regex.matches("abc"));
    EXPECT_TRUE(regex.matches("abbc"));
    EXPECT_TRUE(regex.matches("abbbbbc"));
    EXPECT_FALSE(regex.matches("adc"));
}

TEST_F(RegexTest, PlusQuantifier) {
    auto regex = compile_regex("ab+c");

    EXPECT_FALSE(regex.matches("ac"));
    EXPECT_TRUE(regex.matches("abc"));
    EXPECT_TRUE(regex.matches("abbc"));
    EXPECT_TRUE(regex.matches("abbbbbc"));
}

TEST_F(RegexTest, OptionalQuantifier) {
    auto regex = compile_regex("ab?c");

    EXPECT_TRUE(regex.matches("ac"));
    EXPECT_TRUE(regex.matches("abc"));
    EXPECT_FALSE(regex.matches("abbc"));
}

TEST_F(RegexTest, QuantifierWithClass) {
    auto regex = compile_regex("[a-z]+");

    EXPECT_TRUE(regex.matches("hello"));
    EXPECT_TRUE(regex.matches("a"));
    EXPECT_FALSE(regex.matches(""));
    EXPECT_FALSE(regex.matches("123"));
}

// =============================================================================
// Alternation Tests
// =============================================================================

TEST_F(RegexTest, SimpleAlternation) {
    auto regex = compile_regex("cat|dog");

    EXPECT_TRUE(regex.matches("cat"));
    EXPECT_TRUE(regex.matches("dog"));
    EXPECT_FALSE(regex.matches("bird"));
}

TEST_F(RegexTest, MultipleAlternation) {
    auto regex = compile_regex("a|b|c|d");

    EXPECT_TRUE(regex.matches("a"));
    EXPECT_TRUE(regex.matches("b"));
    EXPECT_TRUE(regex.matches("c"));
    EXPECT_TRUE(regex.matches("d"));
    EXPECT_FALSE(regex.matches("e"));
}

// =============================================================================
// Grouping Tests
// =============================================================================

TEST_F(RegexTest, SimpleGroup) {
    auto regex = compile_regex("(ab)+");

    EXPECT_TRUE(regex.matches("ab"));
    EXPECT_TRUE(regex.matches("abab"));
    EXPECT_TRUE(regex.matches("ababab"));
    EXPECT_FALSE(regex.matches("a"));
    EXPECT_FALSE(regex.matches("aba"));
}

TEST_F(RegexTest, GroupWithAlternation) {
    auto regex = compile_regex("(cat|dog)s");

    EXPECT_TRUE(regex.matches("cats"));
    EXPECT_TRUE(regex.matches("dogs"));
    EXPECT_FALSE(regex.matches("birds"));
}

TEST_F(RegexTest, NestedGroups) {
    auto regex = compile_regex("((ab)+c)+");

    EXPECT_TRUE(regex.matches("abc"));
    EXPECT_TRUE(regex.matches("ababc"));
    EXPECT_TRUE(regex.matches("abcabc"));
}

// =============================================================================
// Escape Tests
// =============================================================================

TEST_F(RegexTest, EscapedDot) {
    auto regex = compile_regex("a\\.b");

    EXPECT_TRUE(regex.matches("a.b"));
    EXPECT_FALSE(regex.matches("aXb"));
}

TEST_F(RegexTest, EscapedStar) {
    auto regex = compile_regex("a\\*b");

    EXPECT_TRUE(regex.matches("a*b"));
    EXPECT_FALSE(regex.matches("ab"));
    EXPECT_FALSE(regex.matches("aaab"));
}

// =============================================================================
// Complex Pattern Tests
// =============================================================================

TEST_F(RegexTest, EmailLike) {
    auto regex = compile_regex("[a-z]+@[a-z]+\\.[a-z]+");

    EXPECT_TRUE(regex.matches("test@example.com"));
    EXPECT_TRUE(regex.matches("user@domain.org"));
    EXPECT_FALSE(regex.matches("invalid"));
    EXPECT_FALSE(regex.matches("@domain.com"));
}

TEST_F(RegexTest, PhoneNumberLike) {
    auto regex = compile_regex("\\d\\d\\d-\\d\\d\\d-\\d\\d\\d\\d");

    EXPECT_TRUE(regex.matches("123-456-7890"));
    EXPECT_FALSE(regex.matches("12-456-7890"));
}

TEST_F(RegexTest, HexColor) {
    auto regex = compile_regex("#[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F]");

    EXPECT_TRUE(regex.matches("#FF00FF"));
    EXPECT_TRUE(regex.matches("#123abc"));
    EXPECT_FALSE(regex.matches("#GGG"));
}

// =============================================================================
// Edge Cases
// =============================================================================

TEST_F(RegexTest, EmptyPattern) {
    auto regex = compile_regex("");

    EXPECT_TRUE(regex.matches(""));
}

TEST_F(RegexTest, SearchInLargeText) {
    auto regex = compile_regex("[0-9]+");
    std::string text = std::string(1000, 'a') + "12345" + std::string(1000, 'b');

    auto pos = regex.search(text);
    ASSERT_TRUE(pos.has_value());
    EXPECT_EQ(*pos, 1000);
}
