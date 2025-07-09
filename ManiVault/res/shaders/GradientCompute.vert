#version 330 core

out vec2 pass_texCoord;

void main() {
    pass_texCoord = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
    gl_Position = vec4(pass_texCoord * 2 - 1, 0, 1);
}