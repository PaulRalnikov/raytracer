cmake_minimum_required(VERSION 3.22.0)
set(CMAKE_CXX_STANDARD 23)

project(render)

find_package(OpenMP REQUIRED)

file(GLOB_RECURSE src "source/*.cpp")

include_directories(
    ${CMAKE_SOURCE_DIR}/source/
    ${CMAKE_SOURCE_DIR}/external/glm
    ${CMAKE_SOURCE_DIR}/external/stb
    ${CMAKE_SOURCE_DIR}/external/rapidjson/include
)

set(TARGET_NAME "${PROJECT_NAME}")

add_executable(${TARGET_NAME} ${src} main.cpp)

if(OpenMP_CXX_FOUND)
    target_link_libraries(${TARGET_NAME} PUBLIC OpenMP::OpenMP_CXX)
endif()
