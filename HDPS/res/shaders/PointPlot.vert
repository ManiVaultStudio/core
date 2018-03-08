#version 330 core

uniform float pointSize;
uniform vec3 selectionColor;

uniform bool selecting;
uniform vec2 start;
uniform vec2 end;

in vec2 vertex;
in vec2 position;
in vec3 color;

out vec2 pass_texCoords;
out vec3 pass_color;

bool inRect(vec2 position, vec2 start, vec2 end)
{
    return position.x > start.x && position.x < end.x && position.y < start.y && position.y > end.y;
}

void main()
{
    pass_color = color;

    if (selecting && inRect(position, start, end))
    {
        pass_color = selectionColor;
    }

    pass_texCoords = vertex;
    gl_Position = vec4(vertex * pointSize + position, 0, 1);
}
