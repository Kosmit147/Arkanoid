#version 430 core

#define COORDINATE_SPACE 4096

layout (location = 0) in vec4 inPosition;

float normalizeCoordinate(float coord)
{
    return coord / COORDINATE_SPACE * 2.0 - 1.0;
}

vec4 normalizeVertexCoordinates(vec4 position)
{
    return vec4(normalizeCoordinate(position.x), normalizeCoordinate(position.y), 1.0, 1.0);
}

void main()
{
    gl_Position = normalizeVertexCoordinates(inPosition);
}