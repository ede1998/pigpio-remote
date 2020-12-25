cmake_minimum_required(VERSION 3.5.0)
project(pigpio-remote-example-i2c-pcf8574-posix)

set(EXTRA_LIBS ${EXTRA_LIBS} lib-pigpio-remote)

set (SRCS main.cpp)

add_compile_definitions(PIGPIO_REMOTE_PLATFORM_POSIX)

add_executable(pigpio-remote-example-i2c-pcf8574-posix ${SRCS})
target_link_libraries(pigpio-remote-example-i2c-pcf8574-posix ${EXTRA_LIBS})