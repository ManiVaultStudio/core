// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#version 330 core

// Scalar effects
#define EFFECT_NONE    0
#define EFFECT_COLOR   1
#define EFFECT_SIZE    2
#define EFFECT_OUTLINE 3
#define EFFECT_COLOR_2D 4

// Point properties
uniform int   scalarEffect;

// Data properties
uniform vec4 dataBounds;

// Selection
uniform int	selectionDisplayMode;				/** Type of selection display (e.g. outline, override) */
uniform float selectionOutlineScale;     		/** Selection outline scale (relative to point size) */
uniform vec3  selectionOutlineColor;			/** Selection outline color (when mode is override) */
uniform bool  selectionOutlineOverrideColor;	/** Whether the selection outline color should be the point color or a custom color */
uniform float selectionOutlineOpacity;     		/** Selection outline opacity */
uniform bool selectionHaloEnabled; 				/** Whether selection outline halo is enabled */
uniform float selectionHaloScale;     			/** Selection halo scale */

// Colormap to use if current effect is EFFECT_COLOR
uniform sampler2D colormap;

// Input variables
smooth in vec2  vTexCoord;
flat   in int   vHighlight;
smooth in float vScalar;
smooth in vec3  vColor;
smooth in float vOpacity;
smooth in vec2  vPosOrig;

// Output color
out vec4 fragColor;

// Perform channel tone mapping
float normalize(float minPixelValue, float maxPixelValue, float pixelValue)
{
    float range		= maxPixelValue - minPixelValue;
    float shift		= pixelValue - minPixelValue;

    return clamp(shift / range, 0.0, 1.0);
}

void main()
{
	bool isHighlighted = vHighlight == 1;

    float len = length(vTexCoord);
	
	float outlineStart = (1.0 / selectionOutlineScale);
	
	if (isHighlighted) {
		switch (selectionDisplayMode) {
			case 0: // Outline
				if (len > 1.0) discard;
				break;
			
			case 1: // Override
				if (len > 1.0) discard;
				break;
		}
	} else {
		switch (selectionDisplayMode) {
			case 0: // Outline
				if (len > outlineStart)	discard;
				break;
			
			case 1: // Override
				if (len > 1.0) discard;
				break;
		}
	}
	
    float a = smoothstep(1, 1 - fwidth(len), len);
    
    // Set point color
    vec3 color = vColor;
    
    if (scalarEffect == EFFECT_COLOR)
        color = texture(colormap, vec2(vScalar, 1 - vScalar)).rgb;

    if (scalarEffect == EFFECT_COLOR_2D)
	{
            float channel1 = normalize(dataBounds[0], dataBounds[1], vPosOrig[0]);
            float channel2 = normalize(dataBounds[2], dataBounds[3], vPosOrig[1]);
            
            color = texture(colormap, vec2(channel1, channel2)).rgb;
	}

	float opacity = 1.0;
	
	switch (selectionDisplayMode) {
		case 0:
		{
			if (vHighlight == 1 && len > outlineStart) {
				if (selectionOutlineOverrideColor)
					color = selectionOutlineColor;
					
				opacity = selectionOutlineOpacity;

				if (selectionHaloEnabled)
					opacity *= 1.0 - smoothstep(outlineStart, 1.0, len);
			} else {
				opacity *= a * vOpacity;
			}		
			
			fragColor = vec4(color, opacity);
			break;
		}
		
		case 1:
		{
			if (vHighlight == 1)
				fragColor = vec4(selectionOutlineColor, a);
			else
				fragColor = vec4(color, a * vOpacity);
				
			break;
		}
	}
}

// For color blending:
// color = (selectionOutlineColor * selectionOutlineOpacity) + (color * (1 - selectionOutlineOpacity));