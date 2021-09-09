#version 330 core

uniform sampler2D colormap;
uniform sampler2D densityMap;

uniform vec2 renderParams;
uniform vec3 colorMapRange;

in vec2 pass_texCoord;

out vec4 fragColor;

// Get the normalized density from the color map range
float toneMapChannel(float minPixelValue, float maxPixelValue, float pixelValue)
{
	return 1.0f;
}

void main() {
	float density = (texture(densityMap, pass_texCoord).r - colorMapRange.x) / colorMapRange.z;
    
	if (density < renderParams.y)
		discard;
    
    vec3 texelSize = vec3(1.0 / 512, 1.0 / 512, 0.0) * 0.5;
    
    float texCoord;
    
    vec4 advancedModeFragment = vec4(1.0);
    
    bool isBoundary = false;
	
    int numSteps = 10;

	// Central differences to find out if we draw the iso contour instead of the color
	vec4 neighborDensities;

	neighborDensities.x = (texture(densityMap, pass_texCoord + texelSize.xz).r - colorMapRange.x) / colorMapRange.z;
	neighborDensities.y = (texture(densityMap, pass_texCoord - texelSize.xz).r - colorMapRange.x) / colorMapRange.z;
	neighborDensities.z = (texture(densityMap, pass_texCoord + texelSize.zy).r - colorMapRange.x) / colorMapRange.z;
	neighborDensities.w = (texture(densityMap, pass_texCoord - texelSize.zy).r - colorMapRange.x) / colorMapRange.z;
	//neighborDensities *= renderParams.x;

	ivec4 stepId = min(ivec4(floor(neighborDensities * vec4(numSteps+1))), ivec4(numSteps));

	isBoundary = (any(notEqual(stepId.xxx, stepId.yzw)));

	texCoord = min(floor(density * (numSteps+1)), numSteps);
	texCoord = clamp(texCoord / numSteps, 0.0, 1.0);
	
    vec3 col = texture(colormap, vec2(texCoord, 1 - texCoord)).rgb;
	
    if(isBoundary) 
    {
        col *= 0.75;
    }
    
    fragColor = vec4(col, 1);
}
