cmake_minimum_required(VERSION 2.8.12)

project(kbest_intervals)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -O3")

include_directories(include)

add_subdirectory(pybind11)

pybind11_add_module(kbest_intervals src/main.cpp)
