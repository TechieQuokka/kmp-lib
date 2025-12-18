/**
 * @file test_pattern.cpp
 * @brief Unit tests for pattern types
 */

#include <gtest/gtest.h>
#include <kmp/kmp.hpp>
#include <string>

using namespace kmp;

class PatternTest : public ::testing::Test {};

// =============================================================================
// Literal Pattern Tests
// =============================================================================

TEST_F(PatternTest, LiteralPatternConstruction) {
    literal_pattern pat("hello");

    EXPECT_EQ(pat.pattern(), "hello");
    EXPECT_EQ(pat.size(), 5);
    EXPECT_FALSE(pat.empty());
}

TEST_F(PatternTest, LiteralPatternEmpty) {
    literal_pattern pat("");

    EXPECT_TRUE(pat.empty());
    EXPECT_EQ(pat.size(), 0);
}

TEST_F(PatternTest, LiteralPatternFailure) {
    literal_pattern pat("ABABAC");

    auto& failure = pat.failure();
    ASSERT_EQ(failure.size(), 6);
    EXPECT_EQ(failure[0], 0);
    EXPECT_EQ(failure[1], 0);
    EXPECT_EQ(failure[2], 1);
    EXPECT_EQ(failure[3], 2);
    EXPECT_EQ(failure[4], 3);
    EXPECT_EQ(failure[5], 0);
}

TEST_F(PatternTest, LiteralPatternIterators) {
    literal_pattern pat("hello");

    std::string result(pat.begin(), pat.end());
    EXPECT_EQ(result, "hello");
}

TEST_F(PatternTest, LiteralPatternIndexAccess) {
    literal_pattern pat("hello");

    EXPECT_EQ(pat[0], 'h');
    EXPECT_EQ(pat[4], 'o');
}

// =============================================================================
// Compile-time Pattern Tests
// =============================================================================

TEST_F(PatternTest, CompiledPatternBasic) {
    constexpr auto pat = compile<"hello">();

    EXPECT_EQ(pat.pattern(), "hello");
    EXPECT_EQ(pat.size(), 5);
}

TEST_F(PatternTest, CompiledPatternFailure) {
    constexpr auto pat = compile<"ABABAC">();

    EXPECT_EQ(pat.failure_at(0), 0);
    EXPECT_EQ(pat.failure_at(1), 0);
    EXPECT_EQ(pat.failure_at(2), 1);
    EXPECT_EQ(pat.failure_at(3), 2);
    EXPECT_EQ(pat.failure_at(4), 3);
    EXPECT_EQ(pat.failure_at(5), 0);
}

TEST_F(PatternTest, CompiledPatternIndexAccess) {
    constexpr auto pat = compile<"hello">();

    EXPECT_EQ(pat[0], 'h');
    EXPECT_EQ(pat[1], 'e');
    EXPECT_EQ(pat[4], 'o');
}

// =============================================================================
// Compile Functions Tests
// =============================================================================

TEST_F(PatternTest, CompileLiteral) {
    auto pat = compile_literal("hello");

    EXPECT_EQ(pat.pattern(), "hello");
    EXPECT_EQ(pat.size(), 5);
}

TEST_F(PatternTest, CompileRegex) {
    auto pat = compile_regex("[a-z]+");

    EXPECT_EQ(pat.source(), "[a-z]+");
    EXPECT_FALSE(pat.empty());
    EXPECT_GT(pat.state_count(), 0);
}
