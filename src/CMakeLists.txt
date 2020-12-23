cmake_minimum_required(VERSION 3.5.0)
project(lib-pigpio-remote)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE SRCS *.cpp)
file(GLOB_RECURSE INT_HDRS *.h)

add_compile_definitions(PIGPIO_REMOTE_PLATFORM_POSIX)

add_library(lib-pigpio-remote STATIC ${SRCS} ${INT_HDRS})