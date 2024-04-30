layout (location = 0) in vec4 inPosition;

out vec2 Position;

void main()
{
    gl_Position = normalizeVertexCoordinates(inPosition);
    Position = vec2(gl_Position);
}
