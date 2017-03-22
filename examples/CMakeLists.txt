cmake_minimum_required(VERSION 2.8)
project(protocol-helper-examples CXX)

include_directories(..)
add_definitions(-Wall -Werror -Wnoexcept -pedantic)

add_executable(mpeg2-ts
  mpeg2_ts.cpp)
target_link_libraries(mpeg2-ts ptl)

add_executable(test-ptl
  test.cpp)
target_link_libraries(test-ptl ptl)
