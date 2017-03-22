cmake_minimum_required(VERSION 2.8)
project(protocol-helper-examples CXX)

include_directories(..)
add_definitions(-Wall -Werror -Wnoexcept -pedantic -std=c++14)

add_executable(mpeg2-ts
  mpeg2_ts.cpp)

add_executable(test-ptl
  test.cpp)
