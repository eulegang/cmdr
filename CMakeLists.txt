cmake_minimum_required(VERSION 3.10)
project(cmdr
  LANGUAGES CXX
  VERSION 0.1.0
  DESCRIPTION "command line processing library"
)

add_library(
  cmdr STATIC
  src/cmdr.cpp
  src/processor.cpp
)

find_package(GTest REQUIRED)

enable_testing()
add_executable(unit tests/cmdr.cpp)

target_include_directories(unit PUBLIC ./src)
target_link_libraries(unit cmdr)
target_link_libraries(unit GTest::gtest_main)


add_test(unit unit)
