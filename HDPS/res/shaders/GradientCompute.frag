#version 330 core

uniform sampler2D densityTexture;

uniform vec2 renderParams;

in vec2 pass_texCoord;

out vec4 fragColor;

void main()
{
    vec3 gradient;
    
    float density = texture(densityTexture, pass_texCoord.xy).r;
    if(density < renderParams.y)
    {
        gradient = vec3(0);
    }
    else
    {
        vec4 neighborDensities;
        neighborDensities.x = textureOffset(densityTexture, pass_texCoord, ivec2(1, 0)).r;
        neighborDensities.y = textureOffset(densityTexture, pass_texCoord, ivec2(-1, 0)).r;
        neighborDensities.z = textureOffset(densityTexture, pass_texCoord, ivec2(0, 1)).r;
        neighborDensities.w = textureOffset(densityTexture, pass_texCoord, ivec2(0, -1)).r;
        neighborDensities *= renderParams.x;

        gradient = vec3(neighborDensities.x - neighborDensities.y, neighborDensities.z - neighborDensities.w, 1);
    }
    
    fragColor = vec4(gradient, density);
}
