#version 330 core

out vec2 uv;

void main() {
	vec2 vertex;

	switch (gl_VertexID) {
		case 0:
			vertex = vec2(-1, -1);
			break;

		case 1:
			vertex = vec2(1, -1);
			break;

		case 2:
			vertex = vec2(1, 1);
			break;

		case 3:
			vertex = vec2(-1, 1);
			break;
	}
	
	gl_Position = vec4(vertex, 0, 1);
	uv = vertex * vec2(0.5, 0.5) + vec2(0.5, 0.5);

	uv.y = 1.0 - uv.y;
}