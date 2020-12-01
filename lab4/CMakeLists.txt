cmake_minimum_required(VERSION 3.17)
project(lab4)

set(CMAKE_CXX_STANDARD 17)

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE thirdparty/catch.hpp)

#add_library(lab4 CSVParser.cpp CSVParser.h)
add_executable(parser main.cpp CSVParser.cpp tuple_print.h)

enable_testing()
add_executable(tests test/test.cpp)
target_link_libraries(tests lab4)
add_test(tests tests)