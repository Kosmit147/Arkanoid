in vec2 Position;

uniform vec2 ballCenter;
uniform float ballRadiusSquared;
uniform vec4 color;

out vec4 outColor;

void main()
{
    const float aaLevel = 0.0001;

    vec2 distVec = Position - ballCenter;
    float distSquared = dot(distVec, distVec);

    if (distSquared > ballRadiusSquared)
        discard;

    float alpha = 1.0 - smoothstep(ballRadiusSquared - aaLevel,
        ballRadiusSquared, distSquared);

    outColor = vec4(color.rgb, alpha * color.a);
}
