#version 330 core

uniform vec2 start;
uniform vec2 end;

void main()
{
    vec2 vertex;

    switch (gl_VertexID) {
    case 0: vertex = vec2(start.x, start.y); break;
    case 1: vertex = vec2(end.x, start.y); break;
    case 2: vertex = vec2(start.x, end.y); break;
    case 3: vertex = vec2(end.x, end.y); break;
    }

    gl_Position = vec4(vertex, 0, 1);
}
