cmake_minimum_required(VERSION 2.8)
project(shell CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(shell shell.cpp)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -O3 -pedantic ")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG ")
endif ()

add_library(shell_lib STATIC launcher.cpp)

target_link_libraries(shell -lncurses shell_lib)
