#version 330 core

uniform sampler2D pdfTexture;

uniform vec2 renderParams;

in vec2 pass_texCoord;

out vec4 fragColor;

void main()
{
    vec2 gradient;
    
    if(texture(pdfTexture, pass_texCoord.xy).r < renderParams.y)
    {
        gradient = vec2(0.0);
    }
    else
    {
        vec4 neighborDensities;
        neighborDensities.x = textureOffset(pdfTexture, pass_texCoord, ivec2(1, 0)).r;
        neighborDensities.y = textureOffset(pdfTexture, pass_texCoord, ivec2(-1, 0)).r;
        neighborDensities.z = textureOffset(pdfTexture, pass_texCoord, ivec2(0, 1)).r;
        neighborDensities.w = textureOffset(pdfTexture, pass_texCoord, ivec2(0, -1)).r;
        neighborDensities *= renderParams.x;

        gradient = vec2(neighborDensities.x - neighborDensities.y, neighborDensities.z - neighborDensities.w);
    }
    
    fragColor = vec4(gradient, 0, 1);
}
