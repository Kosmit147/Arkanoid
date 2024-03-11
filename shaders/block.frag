#version 430 core

uniform float time;

out vec4 outColor;

void main()
{
   float r = sin(time);
   float g = cos(time);

   outColor = vec4(r, g, 1.0, 1.0);
}