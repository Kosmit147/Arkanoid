cmake_minimum_required(VERSION 3.24)

project(arkanoid)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set(CMAKE_COMPILE_WARNING_AS_ERROR On)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE On)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO On)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL On)

file(GLOB_RECURSE ARKANOID_SOURCES src/*.c)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set(SUBSYSTEM "")
    add_compile_definitions(_DEBUG)
elseif(CMAKE_BUILD_TYPE MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "MinSizeRel" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    set(SUBSYSTEM "WIN32")
    add_compile_definitions(_RELEASE)
endif()

add_executable(
    arkanoid ${SUBSYSTEM}
    dependencies/GLAD/src/glad.c
    ${ARKANOID_SOURCES}
)

if (CMAKE_C_COMPILER_ID MATCHES "GNU")
    target_compile_options(arkanoid PRIVATE -Wall -Wextra -Werror -Wconversion)
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(arkanoid PRIVATE -Wall -Wextra -Werror -Wconversion -Wno-c2x-extensions)
endif()

target_include_directories(arkanoid PUBLIC dependencies/GLFW/include)
target_include_directories(arkanoid PUBLIC dependencies/GLAD/include)
target_include_directories(arkanoid PUBLIC dependencies/INCBIN/include)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only")
set(GLFW_INSTALL OFF CACHE BOOL "GLFW lib only")

add_subdirectory(dependencies/GLFW)
target_link_libraries(arkanoid glfw)