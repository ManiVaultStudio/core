#version 330 core

// Scalar effects
#define EFFECT_NONE    0
#define EFFECT_COLOR   1
#define EFFECT_SIZE    2
#define EFFECT_OUTLINE 3

// Point properties
uniform int   scalarEffect;
uniform vec3  outlineColor;

/** Focus selection parameters */
uniform int numSelectedPoints;
uniform bool focusSelection;

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
    float len = length(vTexCoord);
    // If the fragment is outside of the circle discard it
    if (len > 1) discard;

    float edge = fwidth(len);
    float a = smoothstep(1, 1 - edge, len);
    
    // Set point color
    vec3 color = vColor;
	
    if (scalarEffect == EFFECT_COLOR) {
		color = texture(colormap, vec2(vScalar, 1-vScalar)).rgb;
    }
    
    // Change color if point is highlighted
    if (vHighlight == 1) {
        color = len > 0.5 ? outlineColor : color;
    }
    
	bool isHighlighted = vHighlight == 1;
	
	fragColor = vec4(color, a * vOpacity);
}
