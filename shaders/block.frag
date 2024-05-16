in vec2 Position;
in RectBounds BorderBounds;
in vec4 Color;

out vec4 outColor;

void main()
{
    outColor = Color;

    if (!pointInsideRectBounds(Position, BorderBounds))
        outColor = saturateColor(outColor, BLOCK_BORDER_COLOR_SATURATION);
}
