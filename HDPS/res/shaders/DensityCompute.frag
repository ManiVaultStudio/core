#version 330 core

uniform sampler2D gaussSampler;

in vec2 pass_texCoord;

out float value;

void main() {
    value = texture(gaussSampler, pass_texCoord).r;
}
