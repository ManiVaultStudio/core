#version 330 core

uniform float pointSize;
uniform mat3 projMatrix;

in vec2 vertex;
in vec2 position;
in vec3 color;
in uint highlight;

out vec2 pass_texCoords;
out vec3 pass_color;
flat out uint pass_highlight;

void main()
{
    pass_color = color;
    pass_highlight = highlight;

    pass_texCoords = vertex;
    
    float scale = 1.0;
    if (pass_highlight == 1u) {
        scale = 1.2;
    }
    
    vec2 pos = (projMatrix * vec3(position, 1)).xy;
    gl_Position = vec4(vec3(vertex * pointSize * scale + pos, 1), 1);
}
