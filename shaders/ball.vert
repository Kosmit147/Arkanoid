layout (location = 0) in vec4 inPosition;

out vec2 Position;

void main()
{
    gl_Position = normalizeVertexPosition(inPosition);
    Position = vec2(gl_Position);
}
