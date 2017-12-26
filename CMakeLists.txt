cmake_minimum_required(VERSION 3.2)

project(CharConv)

set(CMAKE_CXX_FLAGS "-std=c++1z -Wall -march=native")

add_subdirectory(vendor/benchmark)

add_library(catch vendor/catch.cpp)
target_include_directories(catch PUBLIC vendor)

add_executable(CharConvTest CharConvTest.cpp)
target_link_libraries(CharConvTest catch)
add_test(CharConvTest CharConvTest)

add_executable(CharConvBenchmark CharConvBenchmark.cpp)
target_link_libraries(CharConvBenchmark benchmark)



