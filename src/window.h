/// @file window.h
/// @brief Is responsible for creating the window and context

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

/// @brief Sets up window and context
/// @param title Windows title
/// @param width Windows width
/// @param height Windows height
/// @return Window pointer from glfw
GLFWwindow* setUpWindowAndContext(const char* title, int width, int height);
