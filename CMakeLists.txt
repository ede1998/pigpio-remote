cmake_minimum_required(VERSION 3.5.0)
project(pigpio-remote)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(BUILD_TESTS "Build unit tests" OFF)
option(BUILD_TESTS "Build unit tests" OFF)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE INT_HDRS src/*.h)
file(GLOB_RECURSE HDRS include/*.h)

add_compile_definitions(PIGPIO_REMOTE_PLATFORM_POSIX)

include_directories(pigpio-remote include)
include_directories(pigpio-remote src)

#target_compile_definitions(pigpio-remote INTERFACE)

add_library(pgipio-remote ${SRCS} ${HDRS} ${INT_HDRS})

if (BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests) 
endif()
