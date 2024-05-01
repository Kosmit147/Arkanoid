layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inTranslation;
layout (location = 2) in vec4 inColor;

out vec4 Color;

void main()
{
    Color = inColor;
    gl_Position = vec4(inPosition + inTranslation, 0.0, 1.0);
}
