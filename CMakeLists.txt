cmake_minimum_required(VERSION 3.2)

project(CharConv CXX)

set(CMAKE_CXX_FLAGS "-std=c++1z -Wall -Wextra -pedantic -Wnarrowing -march=native")

option(BENCHMARK_ENABLE_TESTING OFF)
add_subdirectory(vendor/benchmark)

enable_testing()

add_library(catch vendor/catch2/catch.cpp)
target_include_directories(catch PUBLIC vendor)

add_library(CharConv INTERFACE)
target_include_directories(CharConv INTERFACE include)

add_executable(CharConvTest src/CharConvTest.cpp)
target_link_libraries(CharConvTest CharConv catch)
add_test(CharConvTest CharConvTest)

add_executable(CharConvBenchmark src/CharConvBenchmark.cpp)
target_link_libraries(CharConvBenchmark CharConv benchmark)
