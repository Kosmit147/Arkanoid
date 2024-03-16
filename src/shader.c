#include "shader.h"

#include <stdio.h>
#include <stdbool.h>

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

static unsigned int compileShader(const char* shaderSrc, GLenum type)
{
    unsigned int shader = glCreateShader(type);
    const char* srcDataPtr = shaderSrc;
    glShaderSource(shader, 1, &srcDataPtr, NULL);
    glCompileShader(shader);

    return shader;
}

unsigned int createShader(const char* vertexShaderSrc, const char* fragmentShaderSrc)
{
    unsigned int vertexShader = compileShader(vertexShaderSrc, GL_VERTEX_SHADER);
    verifyShaderCompilation(vertexShader);
    unsigned int fragmentShader = compileShader(fragmentShaderSrc, GL_FRAGMENT_SHADER);
    verifyShaderCompilation(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    verifyProgramLinkage(shaderProgram);
    glUseProgram(shaderProgram);

#ifdef NDEBUG
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
#endif

    return shaderProgram;
}