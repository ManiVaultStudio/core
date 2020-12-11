#version 330

uniform sampler2D colorMapTexture;
in vec2 uv;
out vec4 fragmentColor;

void main(void)
{
    //texture(colorMapTexture, vec2(channel, 0));
    return vec4(1, 0, 0, 1);
}