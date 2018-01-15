#version 330 core

uniform sampler2D gaussSampler;

in vec2 pass_texCoord;

out vec4 fragColor;

void main() {
    float f = texture(gaussSampler, pass_texCoord).r;
    fragColor = vec4(f, f, f, 1);
}
