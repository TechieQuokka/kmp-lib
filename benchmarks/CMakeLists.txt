add_executable(kmp_benchmarks
    bench_search.cpp
    bench_simd.cpp
    bench_regex.cpp
)

target_link_libraries(kmp_benchmarks PRIVATE
    kmp::kmp
    benchmark::benchmark
    benchmark::benchmark_main
)

target_compile_features(kmp_benchmarks PRIVATE cxx_std_23)

# Enable SIMD optimizations
if(MSVC)
    target_compile_options(kmp_benchmarks PRIVATE /O2 /arch:AVX2)
else()
    target_compile_options(kmp_benchmarks PRIVATE -O3 -mavx2 -msse4.2)
endif()
