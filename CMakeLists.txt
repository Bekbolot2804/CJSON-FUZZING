cmake_minimum_required(VERSION 4.0)
project(fuzz_cjson C)


add_library(cjson STATIC cJSON.c)
target_include_directories(cjson PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(fuzz_cjson fuzz.c)
target_link_libraries(fuzz_cjson PRIVATE cjson)

target_compile_options(fuzz_cjson PRIVATE -fsanitize=fuzzer,address -g)
target_link_options(fuzz_cjson PRIVATE -fsanitize=fuzzer,address)

target_compile_options(cjson PRIVATE -fsanitize=address)