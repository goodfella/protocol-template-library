cmake_minimum_required(VERSION 2.8)
project(protocol-template-library CXX)

add_library(ptl INTERFACE)
target_compile_options(ptl INTERFACE -std=c++14)
target_include_directories(ptl INTERFACE include)
add_subdirectory(examples)
