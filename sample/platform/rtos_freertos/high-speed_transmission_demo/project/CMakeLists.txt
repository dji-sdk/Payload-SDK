cmake_minimum_required(VERSION 2.8)
project(high-speed_transmission_demo C)

set(CMAKE_EXE_LINKER_FLAGS "-pthread")

add_executable(high-speed_transmission_demo ../application/test_high_speed_transmission.c)