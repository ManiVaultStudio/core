// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#version 330 core

// Scalar effects
#define EFFECT_NONE    0
#define EFFECT_COLOR   1
#define EFFECT_SIZE    2
#define EFFECT_OUTLINE 3
#define EFFECT_COLOR_2D 4



// Point properties
uniform float 	pointSize;        		/** Point size in x- and y direction to account for anisotropy of the render canvas */
uniform vec2 	viewportSize;  			/** (width, height) of viewport */
uniform int   	scalarEffect;
uniform float 	pointOpacity;     		/** Point opacity */
uniform mat4 	mvp;            		/** Projection matrix from bounds space to clip space */
uniform bool 	hasHighlights;     		/** Whether a highlight buffer is used */
uniform bool 	hasFocusHighlights;		/** Whether a focus highlight buffer is used */
uniform bool 	hasScalars;        		/** Whether a scalar buffer is used */
uniform vec3 	colorMapRange;     		/** Color map scalar range */
uniform bool 	hasColors;         		/** Whether a color buffer is used */
uniform bool 	hasSizes;          		/** Whether a point size buffer is used */
uniform bool 	hasOpacities;			/** Whether an opacity buffer is used */

// Selection visualization
uniform int	selectionDisplayMode;		/** Type of selection display (e.g. outline, override) */
uniform float 	selectionOutlineScale;     	/** Selection outline scale */
uniform float 	selectionOutlineOpacity;     	/** Selection outline opacity */
uniform vec3  	selectionOutlineColor;		/** Selection outline color */
uniform bool 	selectionHaloEnabled; 		/** Whether selection halo is enabled */
uniform float 	selectionHaloScale;		/** Selection halo scale */

// Focus visualization
uniform vec3  	focusRegionColor;			/** Focus region color */
uniform float  	focusRegionOpacity;			/** Focus region opacity */
uniform vec3  	focusOutlineColor;			/** Focus outline color */
uniform float 	focusOutlineScale;     		/** Focus outline scale */
uniform float 	focusOutlineOpacity;		/** Focus outline opacity */

uniform bool 	randomizedDepthEnabled;		/** Whether to randomize the z-order */

// Miscellaneous
uniform float 	windowAspectRatio;			/** Window aspect ratio (width / height) */

layout(location = 0) in vec2    vertex;         	/** Vertex input, always a [-1, 1] quad */
layout(location = 1) in vec2    position;       	/** 2-Dimensional positions of points */
layout(location = 2) in int     highlight;      	/** Mask of highlights over the points */
layout(location = 3) in int     focusHighlight;		/** Mask of focus highlights over the points */
layout(location = 4) in float   scalar;         	/** Point scalar */
layout(location = 5) in vec3    color;          	/** Point color */
layout(location = 6) in float   size;           	/** Point size */
layout(location = 7) in float   opacity;        	/** Point opacity */

// Output variables
smooth out vec2  vTexCoord;
flat   out int   vHighlight;
flat   out int   vFocusHighlight;
smooth out float vScalar;
smooth out vec3  vColor;
smooth out float vOpacity;
smooth out vec2  vPosOrig;

/*
float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}
*/

uniform float time;
out vec4 fragment;



// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}



// Compound versions of the hashing algorithm I whipped together.
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }



// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}

// Pseudo-random value in half-open range [0:1].
float random( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }
float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec4  v ) { return floatConstruct(hash(floatBitsToUint(v))); }

void main()
{
	// Use normalized quad vertices as texture coordinates
	vTexCoord = vertex;
	
	// Convert quad size from pixels to normalized device coordinates (NDC)
    vec2 pixelSize = vec2(pointSize) / viewportSize;

    // Apply projection only to the instance position, NOT to the quad size
    vec4 worldPos = mvp * vec4(position, 0.0, 1.0);
	
	vec2 scaledVertex;
	
	if (hasSizes)
        scaledVertex = vertex * (vec2(size) / viewportSize);
	else
		scaledVertex = vertex * pixelSize;
		
	// Scale the vertex based on the selection display mode
	scaledVertex *= ((selectionDisplayMode == 0) ? selectionOutlineScale : 1);
	
    // Keep quad size in screen-space while maintaining correct aspect ratio
    vec2 finalPos = worldPos.xy + scaledVertex;
	
	// Compute random depth
	float depth = randomizedDepthEnabled ? random(worldPos.xy) : 0;
	
	// Set the final position
    gl_Position = vec4(finalPos, depth, 1.0); // Convert to NDC [-1,1]
	
	vHighlight 		= hasHighlights ? highlight : 0;
	vFocusHighlight = hasFocusHighlights ? focusHighlight : 0;
    vScalar 		= hasScalars ? (scalar - colorMapRange.x) / colorMapRange.z : 0;
    vColor 			= hasColors ? color : vec3(0.5);
    vOpacity 		= pointOpacity;
	
    if (hasOpacities)
        vOpacity = opacity;
}
