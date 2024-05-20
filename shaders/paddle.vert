layout (location = 0) in vec4 inPosition;

out vec2 Position;

void main()
{
    Position = vec2(inPosition);
    gl_Position = normalizeVertexPosition(inPosition);
}
