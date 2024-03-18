#version 430 core

out vec4 outColor;

in uniform vec2 ballCenter;
in uniform float radius;

void main()
{
    
    vec2 vec = vec2(ballCenter.x - gl_FragCoord.x, ballCenter.y - gl_FragCoord.y);
    float length = sqrt((vec.x * vec.x + vec.y * vec.y));
    
    if(radius < length){
        discard;
    }

    outColor = vec4(1.0, 1.0, 1.0, 1.0);
}