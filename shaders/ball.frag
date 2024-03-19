#version 430 core

in vec4 Position;

uniform vec2 normalBallCenter;
uniform float normalBallRadiusSquared;

out vec4 outColor;

void main()
{
    vec2 vec = vec2(normalBallCenter.x - Position.x, normalBallCenter.y - Position.y);
    float lenSquared = (vec.x * vec.x + vec.y * vec.y);
    
    if (lenSquared > normalBallRadiusSquared) 
    {
        discard;
    }
    
    outColor = vec4(1.0, 1.0, 1.0, 1.0);   
}