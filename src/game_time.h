#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "helpers.h"

#include "defines.h"

extern float prevTime;
extern float currTime;
extern float deltaTime;
extern float subStepDeltaTime;

static inline void initTime()
{
    currTime = (float)glfwGetTime();
}

static inline void updateTime()
{
    prevTime = currTime;
    currTime = (float)glfwGetTime();
    deltaTime = min(currTime - prevTime, DELTA_TIME_LIMIT);
    subStepDeltaTime = deltaTime / SIMULATION_SUB_STEPS;
}
