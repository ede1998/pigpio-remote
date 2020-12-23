cmake_minimum_required(VERSION 3.5.0)

project(pigpio-remote)

option(BUILD_TESTS "Build unit tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(src)

if (BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if (BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests) 
endif()
