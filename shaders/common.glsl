struct RectBounds
{
    vec2 topLeft;
    vec2 bottomRight;
};

float normalizePosition(float coord)
{
    return coord / float(COORDINATE_SPACE) * 2.0 - 1.0;
}

vec2 normalizeVertexPosition(vec2 vec)
{
    return vec2(normalizePosition(vec.x), normalizePosition(vec.y));
}

vec4 normalizeVertexPosition(vec4 coords)
{
    return vec4(normalizeVertexPosition(coords.xy), 0.0, 1.0);
}

bool pointInsideRectBounds(vec2 point, RectBounds bounds)
{
    return point.x >= bounds.topLeft.x && point.x <= bounds.bottomRight.x
        && point.y <= bounds.topLeft.y && point.y >= bounds.bottomRight.y;
}

vec4 saturateColor(vec4 color, float percentage)
{
    vec4 saturated = color;

    saturated.r *= percentage;
    saturated.g *= percentage;
    saturated.b *= percentage;

    return saturated;
}
