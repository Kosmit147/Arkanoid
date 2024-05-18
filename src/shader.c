#include "shader.h"

#define INCBIN_PREFIX
#include <incbin.h>

#include <stdbool.h>

#include "log.h"

INCTXT(definesShared, "../src/defines_shared.h");

/// @brief The number of sources which comprise a shader's full source code.
#define SHADER_SOURCES_COUNT 6

/// @brief Additional source code which will be inserted before every shader's source code. Set by @ref
/// setCommonShaderSrc.
static const char* commonShaderSrc = "";

void setCommonShaderSrc(const char* src)
{
    commonShaderSrc = src;
}

/// @brief Verifies that a shader was compiled successfully. Logs an error if it wasn't.
/// @param shader Shader ID.
/// @return True if the shader was compiled successfully, false otherwise.
static bool verifyShaderCompilation(unsigned int shader)
{
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        logError("Shader compilation failed: %s.\n", infoLog);
    }

    return success;
}

/// @brief Verifies that a shader (program) was linked successfully. Logs an error if it wasn't.
/// @param shader Program ID.
/// @return True if the program was linked successfully, false otherwise.
static bool verifyProgramLinkage(unsigned int program)
{
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        logError("Shader program linkage failed: %s.\n", infoLog);
    }

    return success;
}

/// @brief Compiles a shader.
/// @param shaderSrc Shader source code.
/// @param type Shader type.
/// @param versionDecl GLSL version directive which will be inserted before the shader's source code.
/// @return Shader ID.
static unsigned int compileShader(const char* shaderSrc, GLenum type, const char* versionDecl)
{
    const char* shaderSources[SHADER_SOURCES_COUNT] = {
        versionDecl,
        definesSharedData,
        "\n", // in case there is no new line at the end of "defines_shared.h" file
        commonShaderSrc,
        "\n", // like above
        shaderSrc,
    };

    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, SHADER_SOURCES_COUNT, shaderSources, NULL);
    glCompileShader(shader);

    return shader;
}

unsigned int createShader(const char* vertexShaderSrc, const char* fragmentShaderSrc, const char* versionDecl)
{
    unsigned int vertexShader = compileShader(vertexShaderSrc, GL_VERTEX_SHADER, versionDecl);
    verifyShaderCompilation(vertexShader);
    unsigned int fragmentShader = compileShader(fragmentShaderSrc, GL_FRAGMENT_SHADER, versionDecl);
    verifyShaderCompilation(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    verifyProgramLinkage(shaderProgram);

#ifndef _DEBUG
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
#endif

    glUseProgram(shaderProgram);
    return shaderProgram;
}
