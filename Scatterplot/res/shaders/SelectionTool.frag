#version 330 core

uniform sampler2D overlayTexture;
in vec2 uv;
out vec4 fragmentColor;

void main(void)
{
    fragmentColor = texture(overlayTexture, uv);
}