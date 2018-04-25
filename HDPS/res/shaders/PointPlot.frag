#version 330 core

uniform float alpha;

in vec2 pass_texCoords;
in vec3 pass_color;
flat in uint pass_highlight;

out vec4 fragColor;

void main()
{
    float len = length(pass_texCoords);
    // If the fragment is outside of the circle discard it
    if (len > 1) discard;

    vec3 color = pass_color;
    vec3 outlineColor = vec3(0.0, 0.0, 1.0);
    
    float edge = fwidth(len);
    float a = smoothstep(1, 1 - edge, len);
    
    if (pass_highlight == 1u) {
        color = len > 0.5 ? outlineColor : color;
    }
    
    fragColor = vec4(color, a * alpha);
}
