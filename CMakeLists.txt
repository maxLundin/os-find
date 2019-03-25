cmake_minimum_required(VERSION 2.8)
project(OS)

set(CMAKE_CXX_STANDARD 17)

include_directories(shell)
add_subdirectory(shell)

include_directories(find)
add_subdirectory(find)
