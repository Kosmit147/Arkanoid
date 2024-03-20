#pragma once

#include <glad/glad.h>

void setExtraShaderSrc(const char* src);
unsigned int createShader(const char* vertexShaderSrc, const char* fragmentShaderSrc, const char* versionDecl);