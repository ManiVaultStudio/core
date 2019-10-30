#version 330 core

// Scalar effects
#define EFFECT_COLOR   0
#define EFFECT_SIZE    1
#define EFFECT_OUTLINE 2

// Point properties
uniform float alpha;
uniform int   scalarEffect;
uniform vec3  outlineColor;

// Colormap to use if useColormap is enabled
uniform bool      useColormap;
uniform sampler2D colormap;

// Input variables
smooth in vec2  vTexCoord;
flat   in int   vHighlight;
smooth in float vScalar;

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
    vec3 color = vec3(0.5, 0.5, 0.5);
    if (scalarEffect == EFFECT_COLOR) {
        color = texture(colormap, vec2(vScalar, 1-vScalar)).rgb;
    }
    
    // Change color if point is highlighted
    if (vHighlight == 1) {
        color = len > 0.5 ? outlineColor : color;
    }
    
    fragColor = vec4(color, a * alpha);
}
