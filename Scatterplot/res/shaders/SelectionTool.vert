#version 330 core

out vec2 uv;

vec2 vertices[4] = vec2[](
	vec2(-1, -1),
	vec2(1, -1),
	vec2(1, 1),
	vec2(-1, 1)
);

void main() {
	vec2 vertex = vertices[gl_VertexID];
	gl_Position = vec4(vertex, 0, 1);
	uv = vertex * vec2(0.5, 0.5) + vec2(0.5, 0.5);
	uv.y = 1.0 - uv.y;
}