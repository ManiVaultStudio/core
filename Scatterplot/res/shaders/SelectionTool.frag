#version 330 core

uniform sampler2D overlayTexture;
in vec2 uv;
out vec4 fragmentColor;

void main(void)
{
    fragmentColor = texture(overlayTexture, uv);

	if (uv.x < 0 && uv.y < 0)
		fragmentColor = vec4(1, 0, 0, 0.5);

	//if (uv.x > 0.5)
	//	fragmentColor = vec4(1, 0, 0, 0.5);
}