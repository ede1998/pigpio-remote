cmake_minimum_required(VERSION 3.5.0)
project(pigpio-remote-example-sos-blink-posix)

set(EXTRA_LIBS ${EXTRA_LIBS} lib-pigpio-remote)

set (SRCS main.cpp)

add_compile_definitions(PIGPIO_REMOTE_PLATFORM_POSIX)

add_executable(pigpio-remote-example-sos-blink-posix ${SRCS})
target_link_libraries(pigpio-remote-example-sos-blink-posix ${EXTRA_LIBS})