#version 330 core

// Scalar effects
#define EFFECT_NONE    0
#define EFFECT_COLOR   1
#define EFFECT_SIZE    2
#define EFFECT_OUTLINE 3

// Point properties
uniform int   scalarEffect;

// Selection
uniform bool selectionOutlineEnabled; 			/** Whether selection outline is enabled */
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
	
    // If the fragment is outside of the circle + outline, discard it
	if (isHighlighted) {
		if (len > 1.0)
			discard;
	} else {
		if (selectionOutlineEnabled) {
			if (len > outlineStart)
				discard;
		} else {
			if (len > 1.0)
				discard;
		}
	}
	
    float edge = fwidth(len);
    float a = smoothstep(1, 1 - edge, len);
    
    // Set point color
    vec3 color = vColor;
    
    if (scalarEffect == EFFECT_COLOR) {
        color = texture(colormap, vec2(vScalar, 1-vScalar)).rgb;
    }
    
	float opacity = 1.0;
	
    // Change color if point is highlighted
    if (vHighlight == 1) {
		if (selectionOutlineEnabled) {
			if (len > outlineStart) {
				if (selectionOutlineOverrideColor)
					color = selectionOutlineColor;
					//color = (selectionOutlineColor * selectionOutlineOpacity) + (color * (1 - selectionOutlineOpacity));
					
				opacity = selectionOutlineOpacity;

				if (selectionHaloEnabled)
					opacity *= 1.0 - smoothstep(outlineStart, 1.0, len);
					//opacity *= ((len - outlineStart) / (1.0 - outlineStart));
			} else {
				if (selectionOutlineOverrideColor)
					color = (selectionOutlineColor * selectionOutlineOpacity) + (color * (1 - selectionOutlineOpacity));
			}
		}
    }
	
    fragColor = vec4(color, opacity * a * vOpacity);
}
