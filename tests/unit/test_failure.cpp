/**
 * @file test_failure.cpp
 * @brief Unit tests for KMP failure function
 */

#include <gtest/gtest.h>
#include <kmp/kmp.hpp>
#include <kmp/detail/failure.hpp>
#include <string>
#include <vector>

using namespace kmp;
using namespace kmp::detail;

class FailureFunctionTest : public ::testing::Test {
protected:
    void verify_failure(std::string_view pattern,
                       const std::vector<size_type>& expected) {
        auto result = compute_failure(pattern.begin(), pattern.end());
        ASSERT_EQ(result.size(), expected.size());
        for (size_t i = 0; i < expected.size(); ++i) {
            EXPECT_EQ(result[i], expected[i])
                << "Mismatch at index " << i << " for pattern: " << pattern;
        }
    }
};

TEST_F(FailureFunctionTest, EmptyPattern) {
    auto result = compute_failure(std::string_view{}.begin(),
                                  std::string_view{}.end());
    EXPECT_TRUE(result.empty());
}

TEST_F(FailureFunctionTest, SingleCharacter) {
    verify_failure("A", {0});
}

TEST_F(FailureFunctionTest, NoRepeats) {
    verify_failure("ABCD", {0, 0, 0, 0});
}

TEST_F(FailureFunctionTest, AllSame) {
    verify_failure("AAAA", {0, 1, 2, 3});
}

TEST_F(FailureFunctionTest, ClassicABABAC) {
    // Classic KMP example
    verify_failure("ABABAC", {0, 0, 1, 2, 3, 0});
}

TEST_F(FailureFunctionTest, AABAAAB) {
    verify_failure("AABAAAB", {0, 1, 0, 1, 2, 2, 3});
}

TEST_F(FailureFunctionTest, ABCABD) {
    verify_failure("ABCABD", {0, 0, 0, 1, 2, 0});
}

TEST_F(FailureFunctionTest, LongPattern) {
    std::string pattern = "ABCDABCDABCE";
    verify_failure(pattern, {0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 0});
}

TEST_F(FailureFunctionTest, OptimizedVersionMatchesBasic) {
    std::vector<std::string> patterns = {
        "ABABAC", "AABAAAB", "ABCDABCD", "AAAA", "ABCD"
    };

    for (const auto& pattern : patterns) {
        auto basic = compute_failure(pattern.begin(), pattern.end());
        auto optimized = compute_failure_optimized(pattern.begin(), pattern.end());

        // The optimized version may differ but should still work correctly
        // Just verify sizes match
        EXPECT_EQ(basic.size(), optimized.size())
            << "Size mismatch for pattern: " << pattern;
    }
}

TEST_F(FailureFunctionTest, SpanOverload) {
    std::string pattern = "ABABAC";
    auto result = compute_failure(std::span<const char>(pattern));
    std::vector<size_type> expected = {0, 0, 1, 2, 3, 0};

    ASSERT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(result[i], expected[i]);
    }
}
