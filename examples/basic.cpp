/**
 * @file basic.cpp
 * @brief Basic usage examples for KMP library
 */

#include <kmp/kmp.hpp>
#include <iostream>
#include <string>

int main() {
    std::cout << "=== KMP Library Basic Examples ===\n\n";

    // ==========================================================================
    // Example 1: Simple string search
    // ==========================================================================
    {
        std::cout << "1. Simple string search:\n";

        std::string text = "Hello, world! Welcome to the world of KMP.";
        std::string pattern = "world";

        auto it = kmp::search(text.begin(), text.end(),
                              pattern.begin(), pattern.end());

        if (it != text.end()) {
            auto pos = std::distance(text.begin(), it);
            std::cout << "   Found '" << pattern << "' at position " << pos << "\n";
        } else {
            std::cout << "   Pattern not found\n";
        }
        std::cout << "\n";
    }

    // ==========================================================================
    // Example 2: Using string_view
    // ==========================================================================
    {
        std::cout << "2. Using string_view:\n";

        auto pos = kmp::search_pos("The quick brown fox jumps", "fox");

        if (pos) {
            std::cout << "   Found 'fox' at position " << *pos << "\n";
        }
        std::cout << "\n";
    }

    // ==========================================================================
    // Example 3: Find all occurrences
    // ==========================================================================
    {
        std::cout << "3. Find all occurrences:\n";

        std::string text = "abracadabra";
        std::string pattern = "abra";

        std::cout << "   Text: \"" << text << "\"\n";
        std::cout << "   Pattern: \"" << pattern << "\"\n";
        std::cout << "   Found at positions: ";

        for (auto pos : kmp::search_all(text, pattern)) {
            std::cout << pos << " ";
        }
        std::cout << "\n\n";
    }

    // ==========================================================================
    // Example 4: Overlapping matches
    // ==========================================================================
    {
        std::cout << "4. Overlapping matches:\n";

        std::string text = "aaaa";
        std::string pattern = "aa";

        auto results = kmp::search_all_vec(text, pattern);

        std::cout << "   Text: \"" << text << "\"\n";
        std::cout << "   Pattern: \"" << pattern << "\"\n";
        std::cout << "   Matches: " << results.size() << "\n";
        std::cout << "   Positions: ";
        for (auto pos : results) {
            std::cout << pos << " ";
        }
        std::cout << "\n\n";
    }

    // ==========================================================================
    // Example 5: Count occurrences
    // ==========================================================================
    {
        std::cout << "5. Count occurrences:\n";

        std::string text = "the cat sat on the mat with the hat";
        std::string pattern = "the";

        auto cnt = kmp::count(text, pattern);
        std::cout << "   Found '" << pattern << "' " << cnt << " times\n\n";
    }

    // ==========================================================================
    // Example 6: Contains check
    // ==========================================================================
    {
        std::cout << "6. Contains check:\n";

        std::string text = "Hello, world!";

        std::cout << "   Contains 'world': "
                  << (kmp::contains(text, "world") ? "yes" : "no") << "\n";
        std::cout << "   Contains 'xyz': "
                  << (kmp::contains(text, "xyz") ? "yes" : "no") << "\n\n";
    }

    // ==========================================================================
    // Example 7: Pre-compiled pattern
    // ==========================================================================
    {
        std::cout << "7. Pre-compiled pattern:\n";

        // Compile once, search multiple times
        auto pattern = kmp::compile_literal("needle");

        std::vector<std::string> haystacks = {
            "looking for a needle in a haystack",
            "no match here",
            "the needle is at the start"
        };

        for (const auto& text : haystacks) {
            auto it = kmp::search(text.begin(), text.end(), pattern);
            if (it != text.end()) {
                std::cout << "   Found in: \"" << text << "\"\n";
            }
        }
        std::cout << "\n";
    }

    // ==========================================================================
    // Example 8: Compile-time pattern (constexpr)
    // ==========================================================================
    {
        std::cout << "8. Compile-time pattern:\n";

        // Pattern computed at compile time
        constexpr auto pat = kmp::compile<"hello">();

        std::string text = "Say hello to the world!";
        auto it = kmp::search(text.begin(), text.end(), pat);

        if (it != text.end()) {
            std::cout << "   Found 'hello' at position "
                      << std::distance(text.begin(), it) << "\n";
        }
        std::cout << "\n";
    }

    // ==========================================================================
    // Example 9: Large text search (SIMD accelerated)
    // ==========================================================================
    {
        std::cout << "9. Large text search (SIMD accelerated):\n";

        // Create a large text
        std::string text(100000, 'a');
        text.replace(99990, 6, "needle");

        auto pos = kmp::search_pos(text, "needle");

        if (pos) {
            std::cout << "   Found 'needle' at position " << *pos
                      << " in 100KB text\n";
        }
        std::cout << "\n";
    }

    std::cout << "=== Done ===\n";
    return 0;
}
