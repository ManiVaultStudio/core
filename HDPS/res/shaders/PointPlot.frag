#version 330 core

uniform float alpha;

in vec2 pass_texCoords;
in vec3 pass_color;

out vec4 fragColor;

void main()
{
    float len = length(pass_texCoords);
    // If the fragment is outside of the circle discard it
    if (len > 1) discard;

    float edge = fwidth(len);
    float a = smoothstep(1, 1 - edge, len);
    fragColor = vec4(pass_color, a * alpha);
}
