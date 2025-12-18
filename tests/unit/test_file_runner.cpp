/**
 * @file test_file_runner.cpp
 * @brief Runs test cases from .txt files and reports detailed results
 */

#include <gtest/gtest.h>
#include <kmp/kmp.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <chrono>

using namespace kmp;

class FileTestRunner : public ::testing::Test {
protected:
    struct TestCase {
        std::string text;
        std::string pattern;
        std::vector<size_t> expected_positions;
        bool expect_not_found;
        int line_number;
    };

    std::vector<TestCase> load_testcases(const std::string& filename) {
        std::vector<TestCase> cases;
        std::ifstream file(filename);

        if (!file.is_open()) {
            std::cerr << "Warning: Could not open " << filename << std::endl;
            return cases;
        }

        std::string line;
        int line_num = 0;
        while (std::getline(file, line)) {
            line_num++;

            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') continue;

            TestCase tc;
            tc.line_number = line_num;
            tc.expect_not_found = false;

            // Parse: TEXT|PATTERN|POSITIONS
            size_t first_delim = line.find('|');
            if (first_delim == std::string::npos) continue;

            size_t second_delim = line.find('|', first_delim + 1);
            if (second_delim == std::string::npos) continue;

            tc.text = line.substr(0, first_delim);
            tc.pattern = line.substr(first_delim + 1, second_delim - first_delim - 1);
            std::string positions_str = line.substr(second_delim + 1);

            // Parse positions
            if (positions_str == "NOT_FOUND" || positions_str == "false") {
                tc.expect_not_found = true;
            } else {
                std::stringstream ss(positions_str);
                std::string pos;
                while (std::getline(ss, pos, ',')) {
                    try {
                        tc.expected_positions.push_back(std::stoull(pos));
                    } catch (...) {
                        // Skip invalid positions
                    }
                }
            }

            cases.push_back(tc);
        }

        return cases;
    }

    std::string truncate_string(const std::string& str, size_t max_len = 60) {
        if (str.length() <= max_len) return str;
        return str.substr(0, max_len) + "...";
    }

    std::string positions_to_string(const std::vector<size_t>& positions) {
        if (positions.empty()) return "NOT_FOUND";
        std::ostringstream oss;
        for (size_t i = 0; i < positions.size(); ++i) {
            if (i > 0) oss << ",";
            oss << positions[i];
        }
        return oss.str();
    }

    void run_testcases(const std::string& filename) {
        auto cases = load_testcases(filename);

        std::cout << "\n";
        std::cout << "================================================================================\n";
        std::cout << "  FILE: " << filename << "\n";
        std::cout << "  TOTAL TEST CASES: " << cases.size() << "\n";
        std::cout << "================================================================================\n\n";

        int passed = 0;
        int failed = 0;
        int test_num = 0;

        auto total_start = std::chrono::high_resolution_clock::now();

        for (const auto& tc : cases) {
            test_num++;

            auto start = std::chrono::high_resolution_clock::now();
            auto results = search_all_vec(tc.text, tc.pattern);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

            bool test_passed = false;

            if (tc.expect_not_found) {
                test_passed = results.empty();
            } else {
                test_passed = (results == tc.expected_positions);
            }

            // Print test case details
            std::cout << "--------------------------------------------------------------------------------\n";
            std::cout << "TEST #" << std::setw(3) << test_num << " [Line " << std::setw(3) << tc.line_number << "]";

            if (test_passed) {
                passed++;
                std::cout << "  [PASSED]  (" << duration.count() << " us)\n";
            } else {
                failed++;
                std::cout << "  [FAILED]  (" << duration.count() << " us)\n";
            }

            std::cout << "  Text    : \"" << truncate_string(tc.text) << "\"\n";
            std::cout << "  Pattern : \"" << tc.pattern << "\"\n";

            std::string expected_str = tc.expect_not_found ? "NOT_FOUND" : positions_to_string(tc.expected_positions);
            std::string actual_str = positions_to_string(results);

            std::cout << "  Expected: " << expected_str << "\n";
            std::cout << "  Actual  : " << actual_str << "\n";

            if (!test_passed) {
                std::cout << "  >>> MISMATCH DETECTED <<<\n";
            }
        }

        auto total_end = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_start);

        std::cout << "\n";
        std::cout << "================================================================================\n";
        std::cout << "  SUMMARY: " << filename << "\n";
        std::cout << "================================================================================\n";
        std::cout << "  Total    : " << cases.size() << " test cases\n";
        std::cout << "  Passed   : " << passed << "\n";
        std::cout << "  Failed   : " << failed << "\n";
        std::cout << "  Pass Rate: " << std::fixed << std::setprecision(1)
                  << (cases.empty() ? 0.0 : 100.0 * passed / cases.size()) << "%\n";
        std::cout << "  Time     : " << total_duration.count() << " ms\n";
        std::cout << "================================================================================\n\n";

        EXPECT_EQ(failed, 0) << "Some test cases failed in " << filename;
    }
};

TEST_F(FileTestRunner, TestCasesFile) {
    run_testcases("tests/data/testcases.txt");
}

TEST_F(FileTestRunner, LongSentencesFile) {
    run_testcases("tests/data/long_sentences.txt");
}

TEST_F(FileTestRunner, StressPatternsFile) {
    run_testcases("tests/data/stress_patterns.txt");
}
