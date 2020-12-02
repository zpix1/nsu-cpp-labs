cmake_minimum_required(VERSION 3.17)
project(lab4)

set(CMAKE_CXX_STANDARD 17)

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE thirdparty/catch.hpp)

add_executable(parser main.cpp)

enable_testing()
add_executable(tests test/test.cpp CSVParser.h)
add_test(tests tests)