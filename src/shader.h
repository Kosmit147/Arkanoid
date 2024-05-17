#pragma once

#include <glad/glad.h>

void setCommonShaderSrc(const char* src);
GLuint createShader(const char* vertexShaderSrc, const char* fragmentShaderSrc, const char* versionDecl);
