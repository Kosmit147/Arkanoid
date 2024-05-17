layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec2 inTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = inTexCoords;
    gl_Position = inPosition;
}
