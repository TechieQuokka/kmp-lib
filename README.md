# KMP String Matching Library

A high-performance C++23 header-only library implementing the Knuth-Morris-Pratt (KMP) string matching algorithm with SIMD acceleration and linear-time regex support.

## Features

- **O(n+m) Time Complexity** - Guaranteed linear-time matching
- **SIMD Acceleration** - Runtime dispatch for SSE4.2, AVX2, and AVX-512
- **Header-Only** - Single include, no linking required
- **C++23 Generators** - Lazy iteration with `std::generator`
- **Compile-Time Patterns** - `constexpr` pattern compilation
- **Linear-Time Regex** - DFA-based regex engine with O(n) guarantee
- **STL Compatible** - Iterator-based interface like `std::search`
- **Thread-Safe** - Concurrent searches with shared patterns

## Requirements

- C++23 compatible compiler (GCC 13+, Clang 17+, MSVC 19.35+)
- CMake 3.25 or higher

## Installation

### CMake FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(
    kmp
    GIT_REPOSITORY https://github.com/TechieQuokka/kmp-lib.git
    GIT_TAG main
)
FetchContent_MakeAvailable(kmp)

target_link_libraries(your_target PRIVATE kmp::kmp)
```

### Manual Installation

```bash
git clone https://github.com/TechieQuokka/kmp-lib.git
cd kmp-lib
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --install build --prefix /usr/local
```

## Quick Start

```cpp
#include <kmp/kmp.hpp>
#include <iostream>

int main() {
    std::string text = "hello world, hello universe";

    // Find first occurrence
    auto pos = kmp::search_pos(text, "hello");
    if (pos) {
        std::cout << "Found at position: " << *pos << "\n";  // 0
    }

    // Find all occurrences
    for (auto p : kmp::search_all(text, "hello")) {
        std::cout << "Match at: " << p << "\n";  // 0, 13
    }

    // Count occurrences
    std::cout << "Count: " << kmp::count(text, "hello") << "\n";  // 2

    // Check existence
    if (kmp::contains(text, "world")) {
        std::cout << "Contains 'world'\n";
    }

    return 0;
}
```

## API Reference

### Search Functions

| Function | Description | Return Type |
|----------|-------------|-------------|
| `search(text, pattern)` | Find first occurrence | `iterator` |
| `search_pos(text, pattern)` | Find first position | `optional<size_t>` |
| `search_all(text, pattern)` | Find all occurrences | `generator<size_t>` |
| `search_all_vec(text, pattern)` | Find all as vector | `vector<size_t>` |
| `count(text, pattern)` | Count occurrences | `size_t` |
| `contains(text, pattern)` | Check if exists | `bool` |

### Pre-compiled Patterns

```cpp
// Runtime literal pattern
auto pattern = kmp::literal_pattern("search term");
auto pos = kmp::search(text.begin(), text.end(), pattern);

// Compile-time pattern (constexpr)
constexpr auto pattern = kmp::compile<"search term">();
auto pos = kmp::search(text.begin(), text.end(), pattern);
```

### Regex Support

```cpp
// Compile regex pattern (DFA-based, O(n) guaranteed)
auto regex = kmp::compile_regex("[a-z]+@[a-z]+\\.[a-z]+");

// Search for match
auto pos = regex.search("contact: test@example.com");  // returns 9

// Check if text matches pattern
bool matches = regex.matches("test@example.com");  // true
```

#### Supported Regex Syntax

| Syntax | Description | Example |
|--------|-------------|---------|
| `.` | Any character | `a.c` matches "abc" |
| `*` | Zero or more | `ab*c` matches "ac", "abc", "abbc" |
| `+` | One or more | `ab+c` matches "abc", "abbc" |
| `?` | Zero or one | `ab?c` matches "ac", "abc" |
| `[abc]` | Character class | `[aeiou]` matches vowels |
| `[a-z]` | Character range | `[0-9]` matches digits |
| `[^abc]` | Negated class | `[^0-9]` matches non-digits |
| `\d` | Digit | `\d+` matches "123" |
| `\w` | Word character | `\w+` matches "hello" |
| `\s` | Whitespace | `\s+` matches spaces |
| `(ab)` | Group | `(ab)+` matches "abab" |
| `a\|b` | Alternation | `cat\|dog` matches either |

## Building from Source

```bash
git clone https://github.com/TechieQuokka/kmp-lib.git
cd kmp-lib

# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release

# Run tests
ctest --test-dir build -C Release --output-on-failure
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `KMP_BUILD_TESTS` | ON | Build unit tests |
| `KMP_BUILD_BENCHMARKS` | ON | Build benchmarks |
| `KMP_BUILD_EXAMPLES` | ON | Build examples |
| `KMP_ENABLE_AVX512` | ON | Enable AVX-512 support |
| `KMP_ENABLE_AVX2` | ON | Enable AVX2 support |
| `KMP_ENABLE_SSE42` | ON | Enable SSE4.2 support |

## Performance

The library automatically selects the optimal implementation at runtime:

| Text Size | Implementation | Throughput |
|-----------|---------------|------------|
| < 64 bytes | Scalar KMP | ~500 MB/s |
| >= 64 bytes | SSE4.2 | ~2 GB/s |
| >= 64 bytes | AVX2 | ~4 GB/s |
| >= 64 bytes | AVX-512 | ~6 GB/s |

*Benchmarks on Intel Core i9-12900K*

## Test Coverage

The library includes comprehensive tests:

- **207 Unit Tests** - All passing
- **144 File-Based Test Cases**
  - `testcases.txt` - 75 basic matching tests
  - `long_sentences.txt` - 50 real-world text tests
  - `stress_patterns.txt` - 19 stress/edge case tests

```bash
# Run all tests
ctest --test-dir build -C Release

# Run specific test suite
./build/tests/Release/kmp_tests --gtest_filter="SearchTest.*"
```

## Project Structure

```
kmp-lib/
├── include/kmp/
│   ├── kmp.hpp           # Main include header
│   ├── search.hpp        # Search functions
│   ├── pattern.hpp       # Pattern types
│   ├── config.hpp        # Configuration
│   └── detail/
│       ├── failure.hpp   # Failure function
│       ├── dfa.hpp       # Regex DFA engine
│       └── simd/
│           ├── dispatch.hpp  # Runtime SIMD dispatch
│           ├── sse42.hpp     # SSE4.2 implementation
│           ├── avx2.hpp      # AVX2 implementation
│           └── avx512.hpp    # AVX-512 implementation
├── tests/
│   ├── unit/             # Unit tests
│   └── data/             # Test case files
├── benchmarks/           # Performance benchmarks
├── examples/             # Usage examples
└── CMakeLists.txt
```

## License

MIT License - see [LICENSE](LICENSE) for details.

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request
