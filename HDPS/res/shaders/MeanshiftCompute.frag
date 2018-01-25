#version 330 core

#define EPSILON 0.001
#define MAX_STEPS 10000

uniform sampler2D gradientTexture;

uniform vec4 renderParams;

in vec2 pass_texCoord;

out vec4 fragColor;

void main()
{
    vec3 texelSize = vec3(renderParams.zw, 0.0);
    
    vec3 gradient = texture(gradientTexture, pass_texCoord).rgb;
    
    if (gradient.xy == 0)
    {
        fragColor = vec4(0, 0, 0, 1);
    }
    else
    {
        float len = length(gradient);
        
        vec2 pos = pass_texCoord;
        vec2 nextPos = pos + normalize(gradient.xy) * texelSize.xy * renderParams.x;
        
        int count = 0;
        for (int i = 0; i < MAX_STEPS; i++)
        {
            if (len < EPSILON && gradient.z > 0.5) break;
            
            pos = nextPos;
            
            gradient = texture(gradientTexture, pos).rgb;
            len = length(gradient);
            
            nextPos = pos + normalize(gradient.xy) * texelSize.xy * renderParams.x;
            
            count = i;
        }
        
        fragColor = vec4(pos, float(count) / (MAX_STEPS-1), 1);
    }
}
