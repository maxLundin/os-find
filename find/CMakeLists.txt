cmake_minimum_required(VERSION 2.8)

project(find)

include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -pedantic -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG ")
endif ()

include_directories(helper)
add_subdirectory(helper)

add_executable(find find.cpp ../Jit/my_function.h)

set_target_properties(filter_lib PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(find filter_lib)
