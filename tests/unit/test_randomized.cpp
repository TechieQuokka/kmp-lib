/**
 * @file test_randomized.cpp
 * @brief Randomized property-based tests for KMP library
 *
 * These tests verify correctness using random inputs and compare
 * results against reference implementations.
 */

#include <gtest/gtest.h>
#include <kmp/kmp.hpp>
#include <string>
#include <vector>
#include <random>
#include <algorithm>

using namespace kmp;

class RandomizedTest : public ::testing::Test {
protected:
    std::mt19937 rng{12345};

    std::string random_string(size_t length, const std::string& alphabet = "abcdefghijklmnopqrstuvwxyz") {
        std::string result;
        result.reserve(length);
        std::uniform_int_distribution<size_t> dist(0, alphabet.size() - 1);
        for (size_t i = 0; i < length; ++i) {
            result += alphabet[dist(rng)];
        }
        return result;
    }

    // Reference implementation using std::search
    std::optional<size_t> reference_search(const std::string& text, const std::string& pattern) {
        if (pattern.empty()) return 0;
        auto it = std::search(text.begin(), text.end(), pattern.begin(), pattern.end());
        if (it == text.end()) return std::nullopt;
        return static_cast<size_t>(std::distance(text.begin(), it));
    }

    // Reference implementation for counting
    size_t reference_count(const std::string& text, const std::string& pattern) {
        if (pattern.empty()) return 0;
        size_t count = 0;
        size_t pos = 0;
        while ((pos = text.find(pattern, pos)) != std::string::npos) {
            ++count;
            ++pos;  // Allow overlapping
        }
        return count;
    }

    // Reference implementation for search_all
    std::vector<size_t> reference_search_all(const std::string& text, const std::string& pattern) {
        std::vector<size_t> results;
        if (pattern.empty()) return results;
        size_t pos = 0;
        while ((pos = text.find(pattern, pos)) != std::string::npos) {
            results.push_back(pos);
            ++pos;  // Allow overlapping
        }
        return results;
    }
};

// =============================================================================
// Property: KMP matches std::search for first occurrence
// =============================================================================

TEST_F(RandomizedTest, MatchesStdSearchSmallAlphabet) {
    for (int i = 0; i < 1000; ++i) {
        std::string text = random_string(100 + i % 900, "ab");
        std::string pattern = random_string(1 + i % 20, "ab");

        auto kmp_result = search_pos(text, pattern);
        auto ref_result = reference_search(text, pattern);

        ASSERT_EQ(kmp_result.has_value(), ref_result.has_value())
            << "Mismatch at iteration " << i
            << "\nText length: " << text.length()
            << "\nPattern: " << pattern;

        if (kmp_result.has_value()) {
            ASSERT_EQ(*kmp_result, *ref_result)
                << "Position mismatch at iteration " << i;
        }
    }
}

TEST_F(RandomizedTest, MatchesStdSearchLargeAlphabet) {
    for (int i = 0; i < 1000; ++i) {
        std::string text = random_string(100 + i % 900);
        std::string pattern = random_string(1 + i % 20);

        auto kmp_result = search_pos(text, pattern);
        auto ref_result = reference_search(text, pattern);

        ASSERT_EQ(kmp_result.has_value(), ref_result.has_value())
            << "Mismatch at iteration " << i;

        if (kmp_result.has_value()) {
            ASSERT_EQ(*kmp_result, *ref_result);
        }
    }
}

// =============================================================================
// Property: Count matches reference count
// =============================================================================

TEST_F(RandomizedTest, CountMatchesReference) {
    for (int i = 0; i < 500; ++i) {
        std::string text = random_string(50 + i % 200, "abc");
        std::string pattern = random_string(1 + i % 5, "abc");

        auto kmp_count = count(text, pattern);
        auto ref_count = reference_count(text, pattern);

        ASSERT_EQ(kmp_count, ref_count)
            << "Count mismatch at iteration " << i
            << "\nText: " << text.substr(0, 50) << "..."
            << "\nPattern: " << pattern
            << "\nKMP: " << kmp_count << ", Ref: " << ref_count;
    }
}

// =============================================================================
// Property: search_all matches reference implementation
// =============================================================================

TEST_F(RandomizedTest, SearchAllMatchesReference) {
    for (int i = 0; i < 500; ++i) {
        std::string text = random_string(50 + i % 200, "ab");
        std::string pattern = random_string(1 + i % 5, "ab");

        auto kmp_results = search_all_vec(text, pattern);
        auto ref_results = reference_search_all(text, pattern);

        ASSERT_EQ(kmp_results.size(), ref_results.size())
            << "Size mismatch at iteration " << i;

        for (size_t j = 0; j < kmp_results.size(); ++j) {
            ASSERT_EQ(kmp_results[j], ref_results[j])
                << "Position mismatch at iteration " << i << ", index " << j;
        }
    }
}

// =============================================================================
// Property: Found positions are valid matches
// =============================================================================

TEST_F(RandomizedTest, FoundPositionsAreValid) {
    for (int i = 0; i < 1000; ++i) {
        std::string text = random_string(100 + i % 500);
        std::string pattern = random_string(1 + i % 15);

        auto results = search_all_vec(text, pattern);

        for (auto pos : results) {
            ASSERT_LE(pos + pattern.size(), text.size())
                << "Position out of bounds";
            ASSERT_EQ(text.substr(pos, pattern.size()), pattern)
                << "Invalid match at position " << pos;
        }
    }
}

// =============================================================================
// Property: Inserted pattern is always found
// =============================================================================

TEST_F(RandomizedTest, InsertedPatternAlwaysFound) {
    for (int i = 0; i < 1000; ++i) {
        std::string text = random_string(100 + i % 500, "abcdefghij");
        std::string pattern = random_string(1 + i % 20, "xyz");  // Disjoint alphabet

        // Insert pattern at random position
        std::uniform_int_distribution<size_t> dist(0, text.size());
        size_t insert_pos = dist(rng);
        text.insert(insert_pos, pattern);

        auto result = search_pos(text, pattern);
        ASSERT_TRUE(result.has_value())
            << "Inserted pattern not found at iteration " << i;
        ASSERT_LE(*result, insert_pos)
            << "Found position should be <= insert position";
    }
}

// =============================================================================
// Property: Empty pattern behavior
// =============================================================================

TEST_F(RandomizedTest, EmptyPatternBehavior) {
    // Test empty pattern behavior only with non-empty text
    // (empty text + empty pattern is a special edge case)
    for (int i = 1; i < 100; ++i) {
        std::string text = random_string(i * 10);

        // Empty pattern should return position 0 (match at beginning)
        auto result = search_pos(text, "");
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(*result, 0);

        // contains with empty pattern should return true
        EXPECT_TRUE(contains(text, ""));

        // count with empty pattern returns 0 (no iteration)
        auto cnt = count(text, "");
        EXPECT_EQ(cnt, 0);
    }
}

// =============================================================================
// Property: Non-existent pattern returns no match
// =============================================================================

TEST_F(RandomizedTest, DisjointAlphabetNoMatch) {
    for (int i = 0; i < 500; ++i) {
        std::string text = random_string(100 + i % 500, "abcde");
        std::string pattern = random_string(1 + i % 10, "xyz");

        auto result = search_pos(text, pattern);
        EXPECT_FALSE(result.has_value())
            << "Found pattern from disjoint alphabet at iteration " << i;
    }
}

// =============================================================================
// Property: Count consistency
// =============================================================================

TEST_F(RandomizedTest, CountEqualsSearchAllSize) {
    for (int i = 0; i < 500; ++i) {
        std::string text = random_string(50 + i % 200, "abc");
        std::string pattern = random_string(1 + i % 5, "abc");

        auto cnt = count(text, pattern);
        auto results = search_all_vec(text, pattern);

        ASSERT_EQ(cnt, results.size())
            << "Count != search_all size at iteration " << i;
    }
}

// =============================================================================
// Property: Contains consistency
// =============================================================================

TEST_F(RandomizedTest, ContainsEqualsSearchPosHasValue) {
    for (int i = 0; i < 1000; ++i) {
        std::string text = random_string(50 + i % 200);
        std::string pattern = random_string(1 + i % 10);

        bool has_match = contains(text, pattern);
        auto pos = search_pos(text, pattern);

        ASSERT_EQ(has_match, pos.has_value())
            << "Consistency mismatch at iteration " << i;
    }
}

// =============================================================================
// Property: First match is minimum position
// =============================================================================

TEST_F(RandomizedTest, FirstMatchIsMinimum) {
    for (int i = 0; i < 500; ++i) {
        std::string text = random_string(100 + i % 200, "ab");
        std::string pattern = random_string(1 + i % 5, "ab");

        auto first = search_pos(text, pattern);
        auto all = search_all_vec(text, pattern);

        if (first.has_value()) {
            ASSERT_FALSE(all.empty());
            ASSERT_EQ(*first, all.front())
                << "First match != search_all front at iteration " << i;
        } else {
            ASSERT_TRUE(all.empty());
        }
    }
}

// =============================================================================
// Property: Positions are strictly increasing
// =============================================================================

TEST_F(RandomizedTest, PositionsStrictlyIncreasing) {
    for (int i = 0; i < 500; ++i) {
        std::string text = random_string(100 + i % 200, "ab");
        std::string pattern = random_string(1 + i % 5, "ab");

        auto results = search_all_vec(text, pattern);

        for (size_t j = 1; j < results.size(); ++j) {
            ASSERT_GT(results[j], results[j-1])
                << "Positions not strictly increasing at iteration " << i;
        }
    }
}

// =============================================================================
// Fuzz: Random length variations
// =============================================================================

TEST_F(RandomizedTest, FuzzVariousLengths) {
    std::uniform_int_distribution<size_t> text_len_dist(0, 1000);
    std::uniform_int_distribution<size_t> pat_len_dist(0, 50);

    for (int i = 0; i < 1000; ++i) {
        size_t text_len = text_len_dist(rng);
        size_t pat_len = pat_len_dist(rng);

        std::string text = random_string(text_len, "abc");
        std::string pattern = random_string(pat_len, "abc");

        // Should not crash
        auto result = search_pos(text, pattern);
        auto all = search_all_vec(text, pattern);
        auto cnt = count(text, pattern);
        auto has = contains(text, pattern);

        // Basic consistency
        EXPECT_EQ(cnt, all.size());
        EXPECT_EQ(has, result.has_value());
    }
}

// =============================================================================
// Regression: Specific edge cases from random testing
// =============================================================================

TEST_F(RandomizedTest, RegressionOverlappingMatches) {
    // Known tricky case: overlapping patterns
    std::string text = "aaaaaaaaa";
    std::string pattern = "aaa";

    auto results = search_all_vec(text, pattern);
    EXPECT_EQ(results.size(), 7);  // 0,1,2,3,4,5,6
}

TEST_F(RandomizedTest, RegressionPatternAtBoundaries) {
    std::string pattern = "xyz";
    std::string text = pattern + random_string(100) + pattern;

    auto results = search_all_vec(text, pattern);
    ASSERT_GE(results.size(), 2);
    EXPECT_EQ(results.front(), 0);
    EXPECT_EQ(results.back(), text.size() - pattern.size());
}

// =============================================================================
// Property: Literal pattern gives same results
// =============================================================================

TEST_F(RandomizedTest, LiteralPatternConsistency) {
    for (int i = 0; i < 500; ++i) {
        std::string text = random_string(100 + i % 200);
        std::string pat_str = random_string(1 + i % 15);

        literal_pattern pat(pat_str);

        auto direct_result = search_pos(text, pat_str);

        // Search with literal_pattern
        auto it = search(text.begin(), text.end(), pat.begin(), pat.end());
        bool found_with_pattern = (it != text.end());

        ASSERT_EQ(direct_result.has_value(), found_with_pattern)
            << "Literal pattern inconsistency at iteration " << i;

        if (direct_result.has_value() && found_with_pattern) {
            size_t pos_with_pattern = static_cast<size_t>(std::distance(text.begin(), it));
            ASSERT_EQ(*direct_result, pos_with_pattern);
        }
    }
}
