CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(protocol-helper-examples CXX)

INCLUDE_DIRECTORIES(..)
ADD_DEFINITIONS(-Wall -Werror -std=c++0x)

ADD_EXECUTABLE(mpeg2-ts
  mpeg2_ts.cpp)
