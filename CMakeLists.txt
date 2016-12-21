cmake_minimum_required(VERSION 3.3)
project(MP2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

set(SOURCE_FILES
        src/ackerman.cpp
        src/memtest.cpp
        src/my_allocator.cpp)

add_executable(MP2 ${SOURCE_FILES})