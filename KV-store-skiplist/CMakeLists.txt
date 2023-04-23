cmake_minimum_required(VERSION 3.20)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_BUILD_TYPE Debug)
project(kvstore)

add_subdirectory(src bin)
add_subdirectory(test)