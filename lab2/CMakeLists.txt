cmake_minimum_required(VERSION 3.17)
project(lab2)

set(CMAKE_CXX_STANDARD 17)

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE thirdparty/catch.hpp)

add_library(lab2 Worker.cpp utility.h)

add_executable(main main.cpp Worker.cpp)

enable_testing()
add_executable(tests test/test.cpp)
target_link_libraries(tests lab2)
add_test(tests tests)