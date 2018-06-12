#version 330 core

uniform sampler2D colormap;
uniform sampler2D densityMap;

uniform vec2 renderParams;

in vec2 pass_texCoord;

out vec4 fragColor;

void main() {
    float density = texture(densityMap, pass_texCoord).r * renderParams.x;
    if (density < renderParams.y) discard;
    
    vec3 texelSize = vec3(1.0 / 512, 1.0 / 512, 0.0) * 0.5;
    
    float texCoord;
    
    vec4 advancedModeFragment = vec4(1.0);
    
    bool isBoundary = false;
    int numSteps = 10;
    if(numSteps > 0)
    {   
        //central differences to find out if we draw the iso contour instead of the color
        vec4 neighborDensities;
        //neighborDensities.x = textureOffset(densityMap, pass_texCoord, ivec2(1, 0)).r;
        //neighborDensities.y = textureOffset(densityMap, pass_texCoord, ivec2(-1, 0)).r;
        //neighborDensities.z = textureOffset(densityMap, pass_texCoord, ivec2(0, 1)).r;
        //neighborDensities.w = textureOffset(densityMap, pass_texCoord, ivec2(0, -1)).r;
        neighborDensities.x = texture(densityMap, pass_texCoord + texelSize.xz).r;
        neighborDensities.y = texture(densityMap, pass_texCoord - texelSize.xz).r;
        neighborDensities.z = texture(densityMap, pass_texCoord + texelSize.zy).r;
        neighborDensities.w = texture(densityMap, pass_texCoord - texelSize.zy).r;
        neighborDensities *= renderParams.x;

        ivec4 stepId = min(ivec4(floor(neighborDensities * vec4(numSteps+1))), ivec4(numSteps));

        isBoundary = (any(notEqual(stepId.xxx, stepId.yzw)));

        texCoord = min(floor(density * (numSteps+1)), numSteps);
        texCoord = clamp(texCoord / numSteps, 0.0, 1.0);
    }
    
    vec3 col = texture(colormap, vec2(texCoord, 0)).rgb;
    if(isBoundary) 
    {
        col *= 0.75;
    }
    
    fragColor = vec4(col, 1);
}
