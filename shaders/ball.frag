#version 430 core

out vec4 outColor;

in vec4 Position;

uniform vec2 ballCenter;
uniform float ballRadius;



void main()
{
    
    vec2 vec = vec2(ballCenter.x - Position.x, ballCenter.y - Position.y);
    float len = sqrt((vec.x * vec.x + vec.y * vec.y));
    
    if(ballRadius < len){
        discard;
    }
    
        
    outColor = vec4(1.0, 1.0, 1.0, 1.0);
    
    
}