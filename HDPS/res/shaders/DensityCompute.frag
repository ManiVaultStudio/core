#version 330 core

uniform sampler2D gaussSampler;

in vec2 pass_texCoord;

out float value;

void main() {
    float f = texture(gaussSampler, pass_texCoord).r;
    value = f;
}
