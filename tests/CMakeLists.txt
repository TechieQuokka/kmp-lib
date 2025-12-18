add_executable(kmp_tests
    unit/test_failure.cpp
    unit/test_search.cpp
    unit/test_pattern.cpp
    unit/test_regex.cpp
    unit/test_simd.cpp
    unit/test_stress.cpp
    unit/test_edge_cases.cpp
    unit/test_randomized.cpp
    unit/test_concurrency.cpp
    unit/test_performance.cpp
    unit/test_file_runner.cpp
)

target_link_libraries(kmp_tests PRIVATE
    kmp::kmp
    GTest::gtest
    GTest::gtest_main
)

target_compile_features(kmp_tests PRIVATE cxx_std_23)

# Note: We do NOT force SIMD flags for tests
# Let runtime dispatch handle SIMD selection based on CPU capabilities
# This avoids illegal instruction exceptions on CPUs without AVX2/AVX-512

include(GoogleTest)
gtest_discover_tests(kmp_tests)
