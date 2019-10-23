#version 330 core

#define EFFECT_COLOR   0
#define EFFECT_SIZE    1
#define EFFECT_OUTLINE 2

uniform float alpha;
uniform int scalarEffect;
uniform sampler2D colormap;
uniform sampler2D texAtlas;

in vec2 pass_texCoords;
flat in uint pass_highlight;
in float pass_scalar;
flat in int pass_instance_id;

out vec4 fragColor;

void main()
{
    // Set point color
    vec3 color = vec3(0.5, 0.5, 0.5);
    if (scalarEffect == EFFECT_COLOR) {
        color = texture(colormap, vec2(pass_scalar, 1-pass_scalar)).rgb;
    }
    int x = pass_instance_id % 250;
    int y = pass_instance_id / 250;
    
    vec2 offset = vec2(x / 250.0, ((y+1) / 250.0));
    vec2 texCoords = (vec2(pass_texCoords.x, pass_texCoords.y) * 0.5 + 0.5) / 250;

    float texCol = texture(texAtlas, offset + vec2(texCoords.x, -texCoords.y)).r;
    
    //if (texCol < 1) discard;
    float subalpha = pass_instance_id % 50 == 0 ? 1 : 0.1;
    fragColor = vec4(color, texCol / 255 * alpha * subalpha);
}
