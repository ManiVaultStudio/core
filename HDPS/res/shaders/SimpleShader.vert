#version 330 core

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec2 position;

out vec2 pass_texCoord;

void main() {
    pass_texCoord = texCoord;
    gl_Position = vec4(vertex*0.05 + position, 0, 1);
}
