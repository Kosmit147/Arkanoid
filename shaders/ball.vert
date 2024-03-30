layout (location = 0) in vec4 inPosition;

out vec4 Position;

void main()
{
    gl_Position = normalizeVertexCoordinates(inPosition);
    Position = gl_Position;
}