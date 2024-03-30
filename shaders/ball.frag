in vec2 Position;

uniform vec2 normalBallCenter;
uniform float normalBallRadiusSquared;

out vec4 outColor;

void main()
{
    float aaLevel = BALL_SHADER_AA_LEVEL;

    vec2 distVec = Position - normalBallCenter;
    float distSquared = dot(distVec, distVec);
    
    if (distSquared > normalBallRadiusSquared) 
        discard;
    
    float alpha = 1.0 - smoothstep(normalBallRadiusSquared - aaLevel, normalBallRadiusSquared, distSquared);

    outColor = vec4(1.0, 1.0, 1.0, alpha);
}