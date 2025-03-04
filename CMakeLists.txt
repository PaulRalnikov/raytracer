cmake_minimum_required(VERSION 3.22.0)
set(CMAKE_CXX_STANDARD 20)

project(render)

# file(GLOB_RECURSE src "source/*.cpp")

set(TARGET_NAME "${PROJECT_NAME}")

add_executable(${TARGET_NAME} main.cpp)
