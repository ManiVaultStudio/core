#version 330 core

// Scalar effects
#define EFFECT_NONE    0
#define EFFECT_COLOR   1
#define EFFECT_SIZE    2
#define EFFECT_OUTLINE 3

// Point properties
uniform int   scalarEffect;

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

// Output color
out vec4 fragColor;

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