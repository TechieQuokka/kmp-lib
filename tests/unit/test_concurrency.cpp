/**
 * @file test_concurrency.cpp
 * @brief Thread safety and concurrency tests for KMP library
 */

#include <gtest/gtest.h>
#include <kmp/kmp.hpp>
#include <string>
#include <vector>
#include <thread>
#include <future>
#include <atomic>
#include <random>
#include <mutex>

using namespace kmp;

class ConcurrencyTest : public ::testing::Test {
protected:
    static constexpr int NUM_THREADS = 8;
    static constexpr int ITERATIONS_PER_THREAD = 100;

    std::string generate_text(size_t length, unsigned seed) {
        std::mt19937 rng(seed);
        std::string result;
        result.reserve(length);
        std::uniform_int_distribution<int> dist('a', 'z');
        for (size_t i = 0; i < length; ++i) {
            result += static_cast<char>(dist(rng));
        }
        return result;
    }
};

// =============================================================================
// Concurrent Read-Only Operations
// =============================================================================

TEST_F(ConcurrencyTest, ConcurrentSearchSameText) {
    const std::string text = generate_text(100'000, 42);
    const std::string pattern = "needle";

    // Insert pattern at known position
    std::string mutable_text = text;
    mutable_text.replace(50'000, pattern.size(), pattern);
    const std::string final_text = mutable_text;

    std::vector<std::future<bool>> futures;
    std::atomic<int> success_count{0};

    for (int i = 0; i < NUM_THREADS; ++i) {
        futures.push_back(std::async(std::launch::async, [&]() {
            for (int j = 0; j < ITERATIONS_PER_THREAD; ++j) {
                auto result = search_pos(final_text, pattern);
                if (result.has_value() && *result == 50'000) {
                    success_count.fetch_add(1, std::memory_order_relaxed);
                }
            }
            return true;
        }));
    }

    for (auto& f : futures) {
        EXPECT_TRUE(f.get());
    }

    EXPECT_EQ(success_count.load(), NUM_THREADS * ITERATIONS_PER_THREAD);
}

TEST_F(ConcurrencyTest, ConcurrentSearchDifferentTexts) {
    std::vector<std::string> texts;
    const std::string pattern = "target";

    for (int i = 0; i < NUM_THREADS; ++i) {
        std::string text = generate_text(10'000, i);
        text.replace(5'000 + i * 100, pattern.size(), pattern);
        texts.push_back(text);
    }

    std::vector<std::future<size_t>> futures;

    for (int i = 0; i < NUM_THREADS; ++i) {
        futures.push_back(std::async(std::launch::async, [&texts, &pattern, i]() {
            size_t found_count = 0;
            for (int j = 0; j < ITERATIONS_PER_THREAD; ++j) {
                auto result = search_pos(texts[i], pattern);
                if (result.has_value()) {
                    ++found_count;
                }
            }
            return found_count;
        }));
    }

    for (auto& f : futures) {
        EXPECT_EQ(f.get(), static_cast<size_t>(ITERATIONS_PER_THREAD));
    }
}

TEST_F(ConcurrencyTest, ConcurrentSearchDifferentPatterns) {
    const std::string text = generate_text(100'000, 42);

    // Create different patterns and insert them
    std::vector<std::string> patterns;
    std::vector<size_t> expected_positions;
    std::string mutable_text = text;

    for (int i = 0; i < NUM_THREADS; ++i) {
        std::string pattern = "PAT" + std::to_string(i) + "END";
        patterns.push_back(pattern);
        size_t pos = 10'000 + i * 10'000;
        mutable_text.replace(pos, pattern.size(), pattern);
        expected_positions.push_back(pos);
    }

    const std::string final_text = mutable_text;
    std::vector<std::future<bool>> futures;

    for (int i = 0; i < NUM_THREADS; ++i) {
        futures.push_back(std::async(std::launch::async,
            [&final_text, &patterns, &expected_positions, i]() {
                for (int j = 0; j < ITERATIONS_PER_THREAD; ++j) {
                    auto result = search_pos(final_text, patterns[i]);
                    if (!result.has_value() || *result != expected_positions[i]) {
                        return false;
                    }
                }
                return true;
            }));
    }

    for (auto& f : futures) {
        EXPECT_TRUE(f.get());
    }
}

// =============================================================================
// Concurrent search_all Operations
// =============================================================================

TEST_F(ConcurrencyTest, ConcurrentSearchAll) {
    // Create text with multiple occurrences
    std::string text;
    const std::string pattern = "MARKER";
    const int num_occurrences = 100;

    for (int i = 0; i < num_occurrences; ++i) {
        text += pattern + generate_text(100, i);
    }

    std::vector<std::future<bool>> futures;

    for (int i = 0; i < NUM_THREADS; ++i) {
        futures.push_back(std::async(std::launch::async, [&text, &pattern]() {
            for (int j = 0; j < ITERATIONS_PER_THREAD / 10; ++j) {
                auto results = search_all_vec(text, pattern);
                if (results.size() != 100) {
                    return false;
                }
            }
            return true;
        }));
    }

    for (auto& f : futures) {
        EXPECT_TRUE(f.get());
    }
}

// =============================================================================
// Concurrent Count Operations
// =============================================================================

TEST_F(ConcurrencyTest, ConcurrentCount) {
    std::string text(10'000, 'a');
    const std::string pattern = "aa";
    const size_t expected_count = 9'999;

    std::atomic<int> correct_count{0};
    std::vector<std::future<void>> futures;

    for (int i = 0; i < NUM_THREADS; ++i) {
        futures.push_back(std::async(std::launch::async, [&]() {
            for (int j = 0; j < ITERATIONS_PER_THREAD; ++j) {
                auto cnt = count(text, pattern);
                if (cnt == expected_count) {
                    correct_count.fetch_add(1, std::memory_order_relaxed);
                }
            }
        }));
    }

    for (auto& f : futures) {
        f.get();
    }

    EXPECT_EQ(correct_count.load(), NUM_THREADS * ITERATIONS_PER_THREAD);
}

// =============================================================================
// Concurrent Contains Operations
// =============================================================================

TEST_F(ConcurrencyTest, ConcurrentContains) {
    const std::string text = "The quick brown fox jumps over the lazy dog";
    std::vector<std::string> patterns = {
        "quick", "brown", "fox", "jumps", "lazy", "dog", "xyz", "notfound"
    };

    std::vector<bool> expected = {
        true, true, true, true, true, true, false, false
    };

    std::vector<std::future<bool>> futures;

    for (int i = 0; i < NUM_THREADS; ++i) {
        futures.push_back(std::async(std::launch::async, [&]() {
            for (int j = 0; j < ITERATIONS_PER_THREAD; ++j) {
                for (size_t k = 0; k < patterns.size(); ++k) {
                    if (contains(text, patterns[k]) != expected[k]) {
                        return false;
                    }
                }
            }
            return true;
        }));
    }

    for (auto& f : futures) {
        EXPECT_TRUE(f.get());
    }
}

// =============================================================================
// Shared literal_pattern (Thread-Safe Pattern Reuse)
// =============================================================================

TEST_F(ConcurrencyTest, SharedLiteralPattern) {
    const literal_pattern pattern("findme");
    const std::string base_text = generate_text(10'000, 42);

    std::vector<std::future<bool>> futures;

    for (int i = 0; i < NUM_THREADS; ++i) {
        futures.push_back(std::async(std::launch::async, [&pattern, &base_text, i]() {
            for (int j = 0; j < ITERATIONS_PER_THREAD; ++j) {
                std::string text = base_text;
                size_t pos = (i * ITERATIONS_PER_THREAD + j) % 9000;
                text.replace(pos, pattern.size(), pattern.pattern());

                auto it = search(text.begin(), text.end(),
                                 pattern.begin(), pattern.end());
                if (it == text.end()) {
                    return false;
                }
            }
            return true;
        }));
    }

    for (auto& f : futures) {
        EXPECT_TRUE(f.get());
    }
}

// =============================================================================
// Shared regex_pattern (Thread-Safe)
// =============================================================================

TEST_F(ConcurrencyTest, SharedRegexPattern) {
    const regex_pattern pattern("[a-z]+@[a-z]+\\.[a-z]+");

    std::vector<std::string> texts = {
        "contact: user@example.com for info",
        "email admin@test.org today",
        "no email here",
        "send to hello@world.net please"
    };

    std::vector<bool> expected = {true, true, false, true};

    std::atomic<int> correct_count{0};
    std::vector<std::future<void>> futures;

    for (int i = 0; i < NUM_THREADS; ++i) {
        futures.push_back(std::async(std::launch::async, [&]() {
            for (int j = 0; j < ITERATIONS_PER_THREAD; ++j) {
                bool all_correct = true;
                for (size_t k = 0; k < texts.size(); ++k) {
                    auto result = pattern.search(texts[k]);
                    if (result.has_value() != expected[k]) {
                        all_correct = false;
                        break;
                    }
                }
                if (all_correct) {
                    correct_count.fetch_add(1, std::memory_order_relaxed);
                }
            }
        }));
    }

    for (auto& f : futures) {
        f.get();
    }

    EXPECT_EQ(correct_count.load(), NUM_THREADS * ITERATIONS_PER_THREAD);
}

// =============================================================================
// High Contention Stress Test
// =============================================================================

TEST_F(ConcurrencyTest, HighContentionStress) {
    const std::string text = generate_text(1'000'000, 42);
    const std::string pattern = "stress";

    // Insert pattern multiple times
    std::string mutable_text = text;
    std::vector<size_t> positions;
    for (size_t i = 0; i < 100; ++i) {
        size_t pos = i * 10'000;
        mutable_text.replace(pos, pattern.size(), pattern);
        positions.push_back(pos);
    }
    const std::string final_text = mutable_text;

    std::atomic<size_t> total_found{0};
    std::vector<std::thread> threads;

    for (int i = 0; i < NUM_THREADS * 2; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < 50; ++j) {
                auto results = search_all_vec(final_text, pattern);
                total_found.fetch_add(results.size(), std::memory_order_relaxed);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // Each thread finds 100 occurrences, 50 times
    size_t expected = NUM_THREADS * 2 * 50 * 100;
    EXPECT_EQ(total_found.load(), expected);
}

// =============================================================================
// Concurrent Pattern Creation
// =============================================================================

TEST_F(ConcurrencyTest, ConcurrentPatternCreation) {
    std::vector<std::future<bool>> futures;

    for (int i = 0; i < NUM_THREADS; ++i) {
        futures.push_back(std::async(std::launch::async, [i]() {
            for (int j = 0; j < ITERATIONS_PER_THREAD; ++j) {
                // Create unique patterns
                std::string pat_str = "pattern" + std::to_string(i * 1000 + j);
                literal_pattern pat(pat_str);

                // Verify failure function is correctly computed
                if (pat.size() != pat_str.size()) {
                    return false;
                }
                if (pat.failure().size() != pat_str.size()) {
                    return false;
                }
            }
            return true;
        }));
    }

    for (auto& f : futures) {
        EXPECT_TRUE(f.get());
    }
}

// =============================================================================
// Concurrent Regex Pattern Creation
// =============================================================================

TEST_F(ConcurrencyTest, ConcurrentRegexCreation) {
    std::vector<std::future<bool>> futures;

    std::vector<std::string> regex_patterns = {
        "[a-z]+",
        "[0-9]+",
        "a*b+c?",
        "(ab|cd)+",
        "\\d+\\.\\d+"
    };

    for (int i = 0; i < NUM_THREADS; ++i) {
        futures.push_back(std::async(std::launch::async, [&regex_patterns, i]() {
            for (int j = 0; j < ITERATIONS_PER_THREAD; ++j) {
                regex_pattern pat(regex_patterns[j % regex_patterns.size()]);
                if (pat.empty()) {
                    return false;
                }
            }
            return true;
        }));
    }

    for (auto& f : futures) {
        EXPECT_TRUE(f.get());
    }
}

// =============================================================================
// Mixed Operations Concurrent
// =============================================================================

TEST_F(ConcurrencyTest, MixedOperationsConcurrent) {
    const std::string text = generate_text(50'000, 42) + "MARKER" +
                             generate_text(50'000, 43);

    std::atomic<int> errors{0};
    std::vector<std::thread> threads;

    // Search threads
    for (int i = 0; i < NUM_THREADS / 2; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < ITERATIONS_PER_THREAD; ++j) {
                auto result = search_pos(text, "MARKER");
                if (!result.has_value() || *result != 50'000) {
                    errors.fetch_add(1, std::memory_order_relaxed);
                }
            }
        });
    }

    // Count threads
    for (int i = 0; i < NUM_THREADS / 4; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < ITERATIONS_PER_THREAD; ++j) {
                auto cnt = count(text, "MARKER");
                if (cnt != 1) {
                    errors.fetch_add(1, std::memory_order_relaxed);
                }
            }
        });
    }

    // Contains threads
    for (int i = 0; i < NUM_THREADS / 4; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < ITERATIONS_PER_THREAD; ++j) {
                if (!contains(text, "MARKER")) {
                    errors.fetch_add(1, std::memory_order_relaxed);
                }
                if (contains(text, "NOTEXIST")) {
                    errors.fetch_add(1, std::memory_order_relaxed);
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(errors.load(), 0);
}
