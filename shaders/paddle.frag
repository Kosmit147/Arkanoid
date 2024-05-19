in vec2 Position;

uniform RectBounds borderRect;
uniform vec4 color;

out vec4 outColor;

void main()
{
    outColor = color;

    if (!pointInsideRectBounds(Position, borderRect))
        outColor = saturateColor(outColor, PADDLE_BORDER_COLOR_SATURATION);
}
