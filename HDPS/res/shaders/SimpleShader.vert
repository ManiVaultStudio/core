#version 330 core

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 position;

void main() {
	gl_Position = vec4(vertex * 50, 0, 1);
}
