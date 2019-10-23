#version 330 core

uniform float pointSize;
uniform mat3 projMatrix;
uniform int scalarEffect;

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 position;
layout(location = 2) in uint highlight;
layout(location = 3) in float scalar;

out vec2 pass_texCoords;
flat out uint pass_highlight;
out float pass_scalar;
flat out int pass_instance_id;

void main()
{
    pass_texCoords = vertex;
    pass_highlight = highlight;
    pass_scalar = scalar;
    pass_instance_id = gl_InstanceID;
    
    float scale = 1.0;
    if (pass_highlight == 1u) {
        scale *= 1.2;
    }
    if (scalarEffect == 1) {
        scale *= scalar;
    }
    
    vec2 pos = (projMatrix * vec3(position, 1)).xy;
    gl_Position = vec4(vec3(vertex * pointSize * scale + pos, 1), 1);
}
