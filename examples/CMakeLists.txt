add_executable(example_basic basic.cpp)
target_link_libraries(example_basic PRIVATE kmp::kmp)
target_compile_features(example_basic PRIVATE cxx_std_23)

add_executable(example_regex regex.cpp)
target_link_libraries(example_regex PRIVATE kmp::kmp)
target_compile_features(example_regex PRIVATE cxx_std_23)

# SIMD flags
if(MSVC)
    target_compile_options(example_basic PRIVATE /arch:AVX2)
    target_compile_options(example_regex PRIVATE /arch:AVX2)
else()
    target_compile_options(example_basic PRIVATE -mavx2)
    target_compile_options(example_regex PRIVATE -mavx2)
endif()
