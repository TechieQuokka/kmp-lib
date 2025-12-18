cmake_minimum_required(VERSION 3.25)
project(kmp-lib
    VERSION 1.0.0
    DESCRIPTION "High-performance KMP string matching library with SIMD and regex support"
    LANGUAGES CXX
)

# C++23 required
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(KMP_BUILD_TESTS "Build unit tests" ON)
option(KMP_BUILD_BENCHMARKS "Build benchmarks" ON)
option(KMP_BUILD_EXAMPLES "Build examples" ON)
option(KMP_ENABLE_AVX512 "Enable AVX-512 support" ON)
option(KMP_ENABLE_AVX2 "Enable AVX2 support" ON)
option(KMP_ENABLE_SSE42 "Enable SSE4.2 support" ON)

# Header-only library
add_library(kmp INTERFACE)
add_library(kmp::kmp ALIAS kmp)

target_include_directories(kmp INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(kmp INTERFACE cxx_std_23)

# SIMD flags (MSVC and GCC/Clang)
if(MSVC)
    if(KMP_ENABLE_AVX512)
        target_compile_options(kmp INTERFACE /arch:AVX512)
    elseif(KMP_ENABLE_AVX2)
        target_compile_options(kmp INTERFACE /arch:AVX2)
    endif()
else()
    if(KMP_ENABLE_AVX512)
        target_compile_options(kmp INTERFACE -mavx512f -mavx512bw)
    elseif(KMP_ENABLE_AVX2)
        target_compile_options(kmp INTERFACE -mavx2)
    elseif(KMP_ENABLE_SSE42)
        target_compile_options(kmp INTERFACE -msse4.2)
    endif()
endif()

# Tests
if(KMP_BUILD_TESTS)
    enable_testing()
    find_package(GTest QUIET)
    if(NOT GTest_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
        )
        FetchContent_MakeAvailable(googletest)
    endif()
    add_subdirectory(tests)
endif()

# Benchmarks
if(KMP_BUILD_BENCHMARKS)
    find_package(benchmark QUIET)
    if(NOT benchmark_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            benchmark
            GIT_REPOSITORY https://github.com/google/benchmark.git
            GIT_TAG v1.8.3
        )
        set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(benchmark)
    endif()
    add_subdirectory(benchmarks)
endif()

# Examples
if(KMP_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Install
include(GNUInstallDirs)
install(TARGETS kmp EXPORT kmp-targets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT kmp-targets
    FILE kmp-config.cmake
    NAMESPACE kmp::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kmp
)
