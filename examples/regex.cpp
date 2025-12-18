/**
 * @file regex.cpp
 * @brief Regex usage examples for KMP library
 */

#include <kmp/kmp.hpp>
#include <iostream>
#include <string>

int main() {
    std::cout << "=== KMP Library Regex Examples ===\n\n";

    // ==========================================================================
    // Example 1: Simple pattern matching
    // ==========================================================================
    {
        std::cout << "1. Simple pattern matching:\n";

        auto regex = kmp::compile_regex("hello");

        std::cout << "   Pattern: 'hello'\n";
        std::cout << "   'hello' matches: "
                  << (regex.matches("hello") ? "yes" : "no") << "\n";
        std::cout << "   'Hello' matches: "
                  << (regex.matches("Hello") ? "yes" : "no") << "\n";
        std::cout << "   DFA states: " << regex.state_count() << "\n\n";
    }

    // ==========================================================================
    // Example 2: Character classes
    // ==========================================================================
    {
        std::cout << "2. Character classes:\n";

        auto regex = kmp::compile_regex("[a-zA-Z]+");

        std::cout << "   Pattern: '[a-zA-Z]+'\n";
        std::cout << "   'Hello' matches: "
                  << (regex.matches("Hello") ? "yes" : "no") << "\n";
        std::cout << "   'Hello123' matches: "
                  << (regex.matches("Hello123") ? "yes" : "no") << "\n";
        std::cout << "   DFA states: " << regex.state_count() << "\n\n";
    }

    // ==========================================================================
    // Example 3: Quantifiers
    // ==========================================================================
    {
        std::cout << "3. Quantifiers (* + ?):\n";

        auto star = kmp::compile_regex("ab*c");
        auto plus = kmp::compile_regex("ab+c");
        auto opt = kmp::compile_regex("ab?c");

        std::cout << "   'ab*c' - 'ac': " << (star.matches("ac") ? "yes" : "no") << "\n";
        std::cout << "   'ab*c' - 'abc': " << (star.matches("abc") ? "yes" : "no") << "\n";
        std::cout << "   'ab*c' - 'abbc': " << (star.matches("abbc") ? "yes" : "no") << "\n";
        std::cout << "   'ab+c' - 'ac': " << (plus.matches("ac") ? "yes" : "no") << "\n";
        std::cout << "   'ab+c' - 'abc': " << (plus.matches("abc") ? "yes" : "no") << "\n";
        std::cout << "   'ab?c' - 'ac': " << (opt.matches("ac") ? "yes" : "no") << "\n";
        std::cout << "   'ab?c' - 'abc': " << (opt.matches("abc") ? "yes" : "no") << "\n";
        std::cout << "   'ab?c' - 'abbc': " << (opt.matches("abbc") ? "yes" : "no") << "\n\n";
    }

    // ==========================================================================
    // Example 4: Alternation
    // ==========================================================================
    {
        std::cout << "4. Alternation (|):\n";

        auto regex = kmp::compile_regex("cat|dog|bird");

        std::cout << "   Pattern: 'cat|dog|bird'\n";
        std::cout << "   'cat' matches: " << (regex.matches("cat") ? "yes" : "no") << "\n";
        std::cout << "   'dog' matches: " << (regex.matches("dog") ? "yes" : "no") << "\n";
        std::cout << "   'fish' matches: " << (regex.matches("fish") ? "yes" : "no") << "\n\n";
    }

    // ==========================================================================
    // Example 5: Shorthand classes
    // ==========================================================================
    {
        std::cout << "5. Shorthand classes (\\d \\w \\s):\n";

        auto digits = kmp::compile_regex("\\d+");
        auto words = kmp::compile_regex("\\w+");
        auto spaces = kmp::compile_regex("\\s+");

        std::cout << "   '\\d+' matches '12345': "
                  << (digits.matches("12345") ? "yes" : "no") << "\n";
        std::cout << "   '\\w+' matches 'hello_123': "
                  << (words.matches("hello_123") ? "yes" : "no") << "\n";
        std::cout << "   '\\s+' matches '   ': "
                  << (spaces.matches("   ") ? "yes" : "no") << "\n\n";
    }

    // ==========================================================================
    // Example 6: Grouping
    // ==========================================================================
    {
        std::cout << "6. Grouping:\n";

        auto regex = kmp::compile_regex("(ab)+");

        std::cout << "   Pattern: '(ab)+'\n";
        std::cout << "   'ab' matches: " << (regex.matches("ab") ? "yes" : "no") << "\n";
        std::cout << "   'abab' matches: " << (regex.matches("abab") ? "yes" : "no") << "\n";
        std::cout << "   'aba' matches: " << (regex.matches("aba") ? "yes" : "no") << "\n\n";
    }

    // ==========================================================================
    // Example 7: Email-like pattern
    // ==========================================================================
    {
        std::cout << "7. Email-like pattern:\n";

        auto regex = kmp::compile_regex("[a-z]+@[a-z]+\\.[a-z]+");

        std::cout << "   Pattern: '[a-z]+@[a-z]+\\.[a-z]+'\n";
        std::cout << "   'user@example.com': "
                  << (regex.matches("user@example.com") ? "valid" : "invalid") << "\n";
        std::cout << "   'invalid': "
                  << (regex.matches("invalid") ? "valid" : "invalid") << "\n";
        std::cout << "   DFA states: " << regex.state_count() << "\n\n";
    }

    // ==========================================================================
    // Example 8: Search in text
    // ==========================================================================
    {
        std::cout << "8. Search in text:\n";

        auto regex = kmp::compile_regex("[0-9]+");
        std::string text = "There are 42 apples and 123 oranges.";

        auto pos = regex.search(text);

        std::cout << "   Text: \"" << text << "\"\n";
        std::cout << "   Pattern: '[0-9]+'\n";
        if (pos) {
            std::cout << "   First match at position: " << *pos << "\n";
        }
        std::cout << "\n";
    }

    // ==========================================================================
    // Example 9: Dot matches any
    // ==========================================================================
    {
        std::cout << "9. Dot (.) matches any character:\n";

        auto regex = kmp::compile_regex("h.llo");

        std::cout << "   Pattern: 'h.llo'\n";
        std::cout << "   'hello': " << (regex.matches("hello") ? "yes" : "no") << "\n";
        std::cout << "   'hallo': " << (regex.matches("hallo") ? "yes" : "no") << "\n";
        std::cout << "   'h9llo': " << (regex.matches("h9llo") ? "yes" : "no") << "\n";
        std::cout << "   'hllo': " << (regex.matches("hllo") ? "yes" : "no") << "\n\n";
    }

    // ==========================================================================
    // Example 10: Negated character class
    // ==========================================================================
    {
        std::cout << "10. Negated character class:\n";

        auto regex = kmp::compile_regex("[^0-9]+");

        std::cout << "    Pattern: '[^0-9]+'\n";
        std::cout << "    'hello': " << (regex.matches("hello") ? "yes" : "no") << "\n";
        std::cout << "    '123': " << (regex.matches("123") ? "yes" : "no") << "\n";
        std::cout << "    'hello123': " << (regex.matches("hello123") ? "yes" : "no") << "\n\n";
    }

    // ==========================================================================
    // Example 11: Complex pattern
    // ==========================================================================
    {
        std::cout << "11. Complex pattern:\n";

        // Simple URL-like pattern
        auto regex = kmp::compile_regex("(http|https)://[a-z]+\\.[a-z]+");

        std::cout << "    Pattern: '(http|https)://[a-z]+\\.[a-z]+'\n";
        std::cout << "    'http://example.com': "
                  << (regex.matches("http://example.com") ? "yes" : "no") << "\n";
        std::cout << "    'https://google.org': "
                  << (regex.matches("https://google.org") ? "yes" : "no") << "\n";
        std::cout << "    'ftp://files.net': "
                  << (regex.matches("ftp://files.net") ? "yes" : "no") << "\n";
        std::cout << "    DFA states: " << regex.state_count() << "\n\n";
    }

    // ==========================================================================
    // Linear Time Guarantee Note
    // ==========================================================================
    {
        std::cout << "=== Linear Time Guarantee ===\n";
        std::cout << "All patterns are compiled to DFA, ensuring O(n) matching.\n";
        std::cout << "No backtracking, no catastrophic performance.\n\n";

        // Demonstrate with a pattern that would be slow in backtracking engines
        auto regex = kmp::compile_regex("a*a*a*a*a*b");
        std::string text(1000, 'a');  // 1000 'a's, no 'b'

        auto pos = regex.search(text);
        std::cout << "Pattern 'a*a*a*a*a*b' on 1000 'a's: "
                  << (pos ? "found" : "not found") << "\n";
        std::cout << "(This would cause catastrophic backtracking in PCRE)\n";
    }

    std::cout << "\n=== Done ===\n";
    return 0;
}
