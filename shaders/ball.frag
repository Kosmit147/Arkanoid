in vec2 Position;

uniform vec2 normalizedBallCenter;
uniform float normalizedBallRadiusSquared;
uniform vec4 color;

out vec4 outColor;

void main()
{
    const float aaLevel = 0.0001;

    vec2 distVec = Position - normalizedBallCenter;
    float distSquared = dot(distVec, distVec);
    
    if (distSquared > normalizedBallRadiusSquared) 
        discard;
    
    float alpha = 1.0 - smoothstep(normalizedBallRadiusSquared - aaLevel,
        normalizedBallRadiusSquared, distSquared);

    outColor = vec4(color.rgb, alpha * color.a);
}
