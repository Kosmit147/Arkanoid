#include "shader.h"

#define INCBIN_PREFIX
#include <incbin.h>

#include <stdio.h>
#include <stdbool.h>

INCTXT(definesShared, "../src/defines_shared.h");

#define SHADER_SOURCES_COUNT 5

static const char* commonShaderSrc = "";

void setCommonShaderSrc(const char* src)
{
    commonShaderSrc = src;
}

static bool verifyShaderCompilation(unsigned int shader)
{
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Shader compilation failed: %s.\n", infoLog);
    }

    return success;
}

static bool verifyProgramLinkage(unsigned int program)
{
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        fprintf(stderr, "Shader program linkage failed: %s.\n", infoLog);
    }

    return success;
}

static unsigned int compileShader(const char* shaderSrc, GLenum type, const char* versionDecl)
{
    const char* shaderSources[SHADER_SOURCES_COUNT] = {
        versionDecl,
        definesSharedData,
        "\n", // in case there is no new line at the end of "defines_shared.h" file
        commonShaderSrc,
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
    glUseProgram(shaderProgram);

#ifndef _DEBUG
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
#endif

    return shaderProgram;
}