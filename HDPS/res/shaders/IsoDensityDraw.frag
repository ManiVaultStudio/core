#version 330 core

uniform sampler1D colorMap;
uniform sampler2D densityMap;

in vec2 pass_texCoord;

out vec4 fragColor;

void main() {
    float density = texture(densityMap, pass_texCoord).r;
    float texCoord;
    
    vec4 advancedModeFragment = vec4(1.0);
    
    bool isBoundary = false;
    int numSteps = 10;
    if(numSteps > 0)
    {   
        //central differences to find out if we draw the iso contour instead of the color
        vec4 neighborDensities;
        neighborDensities.x = textureOffset(densityTexture, pass_texCoord, ivec2(1, 0)).r;
        neighborDensities.y = textureOffset(densityTexture, pass_texCoord, ivec2(-1, 0)).r;
        neighborDensities.z = textureOffset(densityTexture, pass_texCoord, ivec2(0, 1)).r;
        neighborDensities.w = textureOffset(densityTexture, pass_texCoord, ivec2(0, -1)).r;
        vec4 tmpDens = neighborDensities;
        neighborDensities *= renderParams.x;

        ivec4 stepId = min(ivec4(floor(neighborDensities * vec4(numSteps+1))), ivec4(numSteps));

        isBoundary = (any(notEqual(stepId.xxx, stepId.yzw)));

        texCoord = min(floor(density * (numSteps+1)), numSteps);
        texCoord = clamp(texCoord / numSteps, 0.0, 1.0);
        
        if(advancedParams.w > 2.5) advancedModeFragment.rgb = vec3( 1.0 - abs( tmpDens.zx - tmpDens.wy)*renderParams.x*50.0, 1.0 );
        else if(advancedParams.w > 1.5) advancedModeFragment.rgb = 1.0 - vec3( density );
        else if(advancedParams.w > 0.5) advancedModeFragment.rgb = coord2RGB( texture( msSampler, vertexPosition.xy ).rg );
    }
    
    vec3 col = texture(colorMap, texCoord).rgb;
    if(isBoundary) 
    {
        col *= 0.75;
        //alpha = 1.0;
    }
    
    fragColor = vec4(col, 1);
}
