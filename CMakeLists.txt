cmake_minimum_required(VERSION 3.8.2)

set(CMAKE_TOOLCHAIN_FILE Arduino-CMake-NG/cmake/Arduino-Toolchain.cmake)

project(ardumiata)

add_compile_options(-Wall -Wextra -Wpedantic)

arduino_cmake_project(ardumiata BOARD_NAME nano BOARD_CPU atmega328old)

add_arduino_executable(ardumiata
    src/main.cpp
    src/lcdi2c.cpp
    src/ect.cpp
    src/voltage.cpp
    src/buzzer.cpp
    src/display.cpp
)

find_arduino_library(wire_lib Wire)
link_arduino_library(ardumiata wire_lib)
set_target_upload_port(ardumiata /dev/ttyUSB0)
