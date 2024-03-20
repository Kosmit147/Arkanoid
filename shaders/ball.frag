in vec4 Position;

uniform vec2 normalBallCenter;
uniform float normalBallRadiusSquared;

out vec4 outColor;

void main()
{
    vec2 distanceVec = vec2(normalBallCenter.x - Position.x, normalBallCenter.y - Position.y);
    float lenSquared = distanceVec.x * distanceVec.x + distanceVec.y * distanceVec.y;
    
    if (lenSquared > normalBallRadiusSquared) 
    {
        discard;
    }
    
    outColor = vec4(1.0, 1.0, 1.0, 1.0);   
}