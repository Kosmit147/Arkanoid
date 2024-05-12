in vec2 Position;
in Rect BorderRect;
in vec4 Color;

out vec4 outColor;

void main()
{
    outColor = Color;

    if (!pointInsideRect(Position, BorderRect))
        outColor = desaturateColor(outColor, 0.8);
}
