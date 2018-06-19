#version 330 core

#define EFFECT_COLOR   0
#define EFFECT_SIZE    1
#define EFFECT_OUTLINE 2

uniform float alpha;
uniform int scalarEffect;
uniform sampler2D colormap;

in vec2 pass_texCoords;
in vec3 pass_color;
flat in uint pass_highlight;
in float pass_scalar;

out vec4 fragColor;

void main()
{
    float len = length(pass_texCoords);
    // If the fragment is outside of the circle discard it
    if (len > 1) discard;

    float edge = fwidth(len);
    float a = smoothstep(1, 1 - edge, len);
    
    // Set point color
    vec3 color = pass_color;
    if (scalarEffect == EFFECT_COLOR) {
        color = texture(colormap, vec2(pass_scalar, pass_scalar)).rgb;
    }
    vec3 outlineColor = vec3(0.0, 0.0, 1.0);
    
    // Change color if point is highlighted
    if (pass_highlight == 1u) {
        color = len > 0.5 ? outlineColor : color;
    }
    
    fragColor = vec4(color, a * alpha);
}
