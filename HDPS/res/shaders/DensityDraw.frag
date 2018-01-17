#version 330 core

uniform sampler2D tex;
uniform float norm;

in vec2 pass_texCoord;

out vec4 fragColor;

void main() {
    float f = 1 - (texture(tex, pass_texCoord).r * norm);
    fragColor = vec4(vec3(f), 1);
}
