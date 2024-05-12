struct Rect
{
    vec2 topLeft;
    vec2 bottomRight;
};

float normalizeCoordinate(float coord)
{
    return coord / float(COORDINATE_SPACE) * 2.0 - 1.0;
}

vec4 normalizeVertexCoordinates(vec4 position)
{
    return vec4(normalizeCoordinate(position.x), 
        normalizeCoordinate(position.y), 0.0, 1.0);
}

bool pointInsideRect(vec2 point, Rect rect)
{
    return point.x >= rect.topLeft.x && point.x <= rect.bottomRight.x
        && point.y <= rect.topLeft.y && point.y >= rect.bottomRight.y;
}

vec4 desaturateColor(vec4 color, float percentage)
{
    vec4 desaturated = color;

    desaturated.r *= percentage;
    desaturated.g *= percentage;
    desaturated.b *= percentage;

    return desaturated;
}
