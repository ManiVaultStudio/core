#version 330 core

uniform sampler2D colormap;
uniform sampler2D densityMap;

uniform vec2 renderParams;
uniform vec3 colorMapRange;

in vec2 pass_texCoord;

out vec4 fragColor;

// Get the normalized density from the color map range
float getNormalizedDensity(vec2 uv)
{
	float density = texture(densityMap, uv).r;
	return (density - colorMapRange.x) / colorMapRange.z;
}

void main() {
	float density = getNormalizedDensity(pass_texCoord);
    
	if (density < renderParams.y)
		discard;
    
    vec3 texelSize = vec3(1.0 / 512, 1.0 / 512, 0.0) * 0.5;
    
    float colorMapUV;
    
    vec4 advancedModeFragment = vec4(1.0);
    
    bool isBoundary = false;
	
    int numSteps = 10;

	// Central differences to find out if we draw the iso contour instead of the color
	vec4 neighborDensities;

	neighborDensities.x = getNormalizedDensity(pass_texCoord + texelSize.xz);
	neighborDensities.y = getNormalizedDensity(pass_texCoord - texelSize.xz);
	neighborDensities.z = getNormalizedDensity(pass_texCoord + texelSize.zy);
	neighborDensities.w = getNormalizedDensity(pass_texCoord - texelSize.zy);

	ivec4 stepId 	= min(ivec4(floor(neighborDensities * vec4(numSteps+1))), ivec4(numSteps));
	isBoundary 		= (any(notEqual(stepId.xxx, stepId.yzw)));

	// Establish color map texture coordinates
	colorMapUV = min(floor(density * (numSteps+1)), numSteps);
	colorMapUV = clamp(colorMapUV / numSteps, 0.0, 1.0);
	
	// Evaluate the color map
    vec3 color = texture(colormap, vec2(colorMapUV, 1 - colorMapUV)).rgb;
	
    if(isBoundary)
        color *= 0.75;
    
	// Set output color
    fragColor = vec4(color, 1);
}
