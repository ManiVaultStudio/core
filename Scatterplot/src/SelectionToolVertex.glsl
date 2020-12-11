#version 330

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec2 texCoord;

out vec2 uv;

//uniform mat4 transform;

void main(void)
{
    //gl_Position = transform * vertex;
    gl_Position = vertex;
    uv = texCoord;
}