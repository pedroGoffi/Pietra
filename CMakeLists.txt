cmake_minimum_required(VERSION 3.27)
project(
    PI
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION Programming Language
)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# General
add_executable(pi src/main.cpp)
