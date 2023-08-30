cmake_minimum_required(VERSION 3.20)
project(main LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(main main.cpp)
