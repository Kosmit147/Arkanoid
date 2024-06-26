layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inTranslation;
layout (location = 2) in vec2 inBorderBoundsTopLeft;
layout (location = 3) in vec2 inBorderBoundsBottomRight;
layout (location = 4) in vec4 inColor;

out vec2 Position;
out RectBounds BorderBounds;
out vec4 Color;

void main()
{
    BorderBounds = RectBounds(inBorderBoundsTopLeft, inBorderBoundsBottomRight);
    Color = inColor;

    Position = inPosition + inTranslation;
    gl_Position = vec4(Position, 0.0, 1.0);
}
