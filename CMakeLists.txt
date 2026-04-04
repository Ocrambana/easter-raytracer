cmake_minimum_required(VERSION 3.5.0)
project(
    Easter-Raytracer
    VERSION 1.0.0
    DESCRIPTION "Raytracer started during the easter weekend"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

include_directories(${PROJECT_SOURCE_DIR}/src/)

file(GLOB PROJECT_SRC "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_executable(${PROJECT_NAME} ${PROJECT_SRC})