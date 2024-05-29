cmake_minimum_required(VERSION 3.8.2)

set(CMAKE_TOOLCHAIN_FILE Arduino-CMake-NG/cmake/Arduino-Toolchain.cmake)
option(DEBUG_DISPLAY OFF)

project(ardumiata)

add_compile_options(-Wall -Wextra -Wpedantic)

arduino_cmake_project(ardumiata BOARD_NAME nano BOARD_CPU atmega328)

add_arduino_executable(ardumiata
    src/main.cpp
    src/lcdi2c.cpp
    src/sensors.cpp
    src/voltage.cpp
    src/buzzer.cpp
    src/display_release.cpp
    src/display_debug.cpp
)

target_compile_options(ardumiata PRIVATE -std=c++20) # Overrides arduino toolchain defaults

if(DEBUG_DISPLAY)
    target_compile_definitions(ardumiata PUBLIC DEBUG_DISPLAY)
endif()

find_arduino_library(wire_lib Wire)
link_arduino_library(ardumiata wire_lib)
set_target_upload_port(ardumiata /dev/ttyUSB0 )
