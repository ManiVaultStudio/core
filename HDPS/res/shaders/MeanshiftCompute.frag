#version 330 core

#define EPSILON 0.002
#define MAX_STEPS 1000

uniform sampler2D gradientTexture;

uniform vec4 renderParams;

in vec2 pass_texCoord;

out vec4 fragColor;

void main()
{
    vec2 texelSize = renderParams.zw;
    
    vec3 t = texture(gradientTexture, pass_texCoord).xyz;
    vec2 gradient = t.xy;
    float density = t.z;
    float threshold = EPSILON * renderParams.y;
    
    float len = length(gradient);
    
    if (density < threshold) {
        fragColor = vec4(0, 0, 0, 1);
        return;
    }
    
    vec2 pos = pass_texCoord;
    //vec2 nextPos = pos + normalize(gradient.xy) * texelSize.xy * renderParams.x;
    
    int count = 0;
    for (int i = 0; i < MAX_STEPS; i++)
    {
        //pos = nextPos;

        vec2 ngradient = texture(gradientTexture, pos).xy;
        if (dot(ngradient, gradient) < 0) break;
        gradient = ngradient;
        //len = length(gradient);
        
        pos = pos + normalize(gradient) * texelSize;// * renderParams.x;
        
        count = i;
    }

    fragColor = vec4(pos, count / (MAX_STEPS-1), 1);
}
