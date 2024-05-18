cmake_minimum_required(VERSION 3.24)

project(arkanoid VERSION 0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_C_EXTENSIONS Off)
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
    ${ARKANOID_SOURCES}
)

if (CMAKE_C_COMPILER_ID MATCHES ".*GNU.*")
    target_compile_options(
        arkanoid PRIVATE
        -Wall -Wextra -Werror -Wconversion -Wpedantic -Wshadow
        -Wdouble-promotion -Wsign-conversion
        -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches
        -fanalyzer
        -Wno-analyzer-possible-null-dereference
        -Wno-analyzer-null-dereference
        -Wno-analyzer-malloc-leak
        -Wno-analyzer-null-argument
        -ffast-math
    )
elseif(CMAKE_C_COMPILER_ID MATCHES ".*Clang.*")
    target_compile_options(
        arkanoid PRIVATE
        -Wall -Wextra -Werror -Wconversion -Wpedantic -Wshadow
        -Wdouble-promotion -Wsign-conversion
        -ffast-math
    )
endif()

target_include_directories(arkanoid PUBLIC dependencies/GLAD/include)
target_include_directories(arkanoid PUBLIC dependencies/GLFW/include)
target_include_directories(arkanoid PUBLIC dependencies/INCBIN/include)
target_include_directories(arkanoid PUBLIC dependencies/STB_IMAGE/include)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only")
set(GLFW_INSTALL OFF CACHE BOOL "GLFW lib only")
option(GLFW_BUILD_X11 OFF)
option(GLFW_BUILD_WAYLAND OFF)
option(DRAW_QUAD_TREE OFF)

if(GLFW_BUILD_WAYLAND)
    target_compile_definitions(arkanoid PRIVATE _GLFW_WAYLAND)
elseif(GLFW_BUILD_X11)
    target_compile_definitions(arkanoid PRIVATE _GLFW_X11)
endif()

if(DRAW_QUAD_TREE)
    target_compile_definitions(arkanoid PRIVATE DRAW_QUAD_TREE)
endif()

add_subdirectory(dependencies/GLAD)
add_subdirectory(dependencies/GLFW)
add_subdirectory(dependencies/STB_IMAGE)
target_link_libraries(arkanoid glad)
target_link_libraries(arkanoid glfw)
target_link_libraries(arkanoid stb_image)
