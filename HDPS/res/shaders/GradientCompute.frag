#version 330 core

uniform sampler2D pdfTexture;

uniform vec4 renderParams;

in vec2 pass_texCoord;

out vec4 fragColor;

void main()
{
    vec3 texelSize = vec3(renderParams.xy, 0);
    
    vec2 gradient;
    
    if(texture(pdfSampler, vertexPosition.xy).r < renderParams.w)
    {
        gradient = vec2(0.0);
    }
    else
    {
        vec4 neighborDensities;
        neighborDensities.x = texture(pdfSampler, vertexPosition.xy + texelSize.xz).r;
        neighborDensities.y = texture(pdfSampler, vertexPosition.xy - texelSize.xz).r;
        neighborDensities.z = texture(pdfSampler, vertexPosition.xy + texelSize.zy).r;
        neighborDensities.w = texture(pdfSampler, vertexPosition.xy - texelSize.zy).r;
        neighborDensities *= renderParams.z;

        gradient = vec2(neighborDensities.x - neighborDensities.y, neighborDensities.z - neighborDensities.w);
    }
    
    fragColor = vec4(gradient, 0, 1);
}
