cmake_minimum_required(VERSION 3.22.0)
set(CMAKE_CXX_STANDARD 23)

project(render)

file(GLOB_RECURSE src "source/*.cpp")

include_directories(
    ${CMAKE_SOURCE_DIR}/source/
    ${CMAKE_SOURCE_DIR}/external/glm
    ${CMAKE_SOURCE_DIR}/external/stb
)

set(TARGET_NAME "${PROJECT_NAME}")

add_executable(${TARGET_NAME} ${src} main.cpp)
