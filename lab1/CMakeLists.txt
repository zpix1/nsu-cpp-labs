cmake_minimum_required(VERSION 3.17)
project(lab1)
set($ENV{LANG} C)
add_subdirectory(Google_tests)

set(CMAKE_CXX_STANDARD 14)
add_library(TritSet__lib TritSet.cpp)
add_executable(lab1 main.cpp)
target_link_libraries(lab1 TritSet__lib)