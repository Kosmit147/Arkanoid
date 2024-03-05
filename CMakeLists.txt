cmake_minimum_required(VERSION 3.24)

project(arkanoid)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set(CMAKE_COMPILE_WARNING_AS_ERROR On)

add_executable(
    arkanoid
    dependencies/GLAD/src/glad.c
    src/main.c
)
target_compile_options(arkanoid PRIVATE -Wall -Wextra -Werror -Wconversion)

target_include_directories(arkanoid PUBLIC dependencies/GLFW/include)
target_include_directories(arkanoid PUBLIC dependencies/GLAD/include)

set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )

add_subdirectory(dependencies/GLFW)
target_link_libraries(arkanoid glfw)