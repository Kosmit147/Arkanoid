#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdbool.h>

GLFWwindow* setUpWindow(const char* title, int width, int height);
bool loadGlad();
void onWindowResize(GLFWwindow* window, int width, int height);
void resetWindowViewport(GLFWwindow* window);
