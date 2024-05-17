/// @file shader.h
/// @brief Functionality related to OpenGL shaders.

#pragma once

#include <glad/glad.h>

/// @brief Set the source code which will be inserted before every shader's source code.
/// @param src Pointer to the source code which will be inserted before every shader's source code. Must
/// remain valid as long as additional shaders are created and no other calls to this function are made.
void setCommonShaderSrc(const char* src);

/// @brief Creates an OpenGL shader (program).
/// @param vertexShaderSrc Vertex shader source code without GLSL version directive.
/// @param fragmentShaderSrc Fragment shader source code without GLSL version directive.
/// @param versionDecl GLSL version directive which will be inserted before vertex and fragment shaders'
/// source code.
/// @return ID of the created shader.
unsigned int createShader(const char* vertexShaderSrc, const char* fragmentShaderSrc,
    const char* versionDecl);
