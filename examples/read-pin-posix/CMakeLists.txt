cmake_minimum_required(VERSION 3.5.0)
project(pigpio-remote-example-read-pin-posix)

set(EXTRA_LIBS ${EXTRA_LIBS} lib-pigpio-remote)

set (SRCS main.cpp)

add_compile_definitions(PIGPIO_REMOTE_PLATFORM_POSIX)

add_executable(pigpio-remote-example-read-pin-posix ${SRCS})
target_link_libraries(pigpio-remote-example-read-pin-posix ${EXTRA_LIBS})