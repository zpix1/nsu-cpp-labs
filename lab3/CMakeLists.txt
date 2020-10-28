cmake_minimum_required(VERSION 3.17)
project(lab3)

set(CMAKE_CXX_STANDARD 17)

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE thirdparty/catch.hpp)

add_library(lab3 Gamer.cpp GameView.cpp utils.h)

add_executable(game
        Gamer.cpp
        main.cpp utils.h GameView.cpp)

enable_testing()
add_executable(tests test/test.cpp)
target_link_libraries(tests lab3)
add_test(tests tests)