in vec2 TexCoords;

out vec4 outColor;

uniform sampler2D tex;

void main()
{
    outColor = texture(tex, TexCoords);
}
