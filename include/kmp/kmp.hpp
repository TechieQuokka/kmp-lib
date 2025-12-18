#pragma once

/**
 * @file kmp.hpp
 * @brief KMP Library - High-performance string matching with SIMD and regex
 *
 * A C++23 header-only library implementing the Knuth-Morris-Pratt algorithm
 * with SIMD acceleration (SSE4.2, AVX2, AVX-512) and linear-time regex support.
 *
 * Key Features:
 *   - O(n+m) time complexity guaranteed
 *   - SIMD-accelerated character scanning
 *   - STL-compatible iterator interface
 *   - Compile-time pattern compilation (constexpr)
 *   - Linear-time regex engine (DFA-based)
 *   - C++23 generator for lazy iteration
 *
 * Basic Usage:
 * @code
 *   #include <kmp/kmp.hpp>
 *
 *   // Simple search
 *   std::string text = "hello world";
 *   auto it = kmp::search(text.begin(), text.end(),
 *                         "world", "world" + 5);
 *
 *   // Using string_view
 *   auto pos = kmp::search_pos("hello world", "world");  // returns 6
 *
 *   // Find all matches
 *   for (auto pos : kmp::search_all("ababa", "aba")) {
 *       std::cout << pos << "\n";  // prints 0, 2
 *   }
 *
 *   // Compile-time pattern
 *   constexpr auto pat = kmp::compile<"pattern">();
 *   auto result = kmp::search(text.begin(), text.end(), pat);
 *
 *   // Regex matching (linear time)
 *   auto regex = kmp::compile_regex("[a-z]+@[a-z]+\\.[a-z]+");
 *   auto match = regex.search("email: test@example.com");
 * @endcode
 *
 * @author KMP Library
 * @version 1.0.0
 * @copyright MIT License
 */

// Configuration and platform detection
#include "config.hpp"

// Core search functionality
#include "search.hpp"

// Pattern types (literal and regex)
#include "pattern.hpp"

// Additional namespace-level documentation
namespace kmp {

/**
 * @namespace kmp
 * @brief KMP string matching library
 *
 * This namespace contains all public APIs for the KMP library:
 *
 * **Search Functions:**
 *   - search()       - Find first occurrence
 *   - search_pos()   - Find first occurrence (returns position)
 *   - search_all()   - Find all occurrences (generator)
 *   - search_all_vec() - Find all occurrences (vector)
 *   - count()        - Count occurrences
 *   - contains()     - Check if pattern exists
 *
 * **Pattern Types:**
 *   - literal_pattern - Pre-compiled literal pattern
 *   - regex_pattern   - Compiled regex (DFA)
 *   - compiled_pattern<> - Compile-time pattern
 *
 * **Factory Functions:**
 *   - compile<"pattern">() - Create compile-time pattern
 *   - compile_literal()    - Create runtime literal pattern
 *   - compile_regex()      - Create runtime regex pattern
 *
 * @see search.hpp for search functions
 * @see pattern.hpp for pattern types
 */

} // namespace kmp
