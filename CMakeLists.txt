cmake_minimum_required(VERSION 3.5.0)

project(pigpio-remote)

option(BUILD_TESTS "Build unit tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)
option(RUN_CODE_ANALYSIS "Run clang tidy during build" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/include)

if (RUN_CODE_ANALYSIS)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  set(CMAKE_CXX_CLANG_TIDY clang-tidy;)
endif()

add_subdirectory(src)

if (BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if (BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests) 
endif()
