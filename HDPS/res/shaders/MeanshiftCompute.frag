// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#version 330 core

#define EPSILON 0.001
#define MAX_STEPS 10000

uniform sampler2D gradientTexture;

uniform vec4 renderParams;

in vec2 pass_texCoord;

out vec4 fragColor;

void main()
{
    vec2 texelSize = renderParams.zw;
    
    vec2 gradient = texture(gradientTexture, pass_texCoord).xy;

    if( all(equal(gradient, vec2(0.0))) )
    {
        fragColor = vec4(0.0);
        return;
    }
    
    vec2 pos = pass_texCoord;
    
    int count = 0;
    for (int i = 0; i < MAX_STEPS; i++)
    {
        gradient = texture(gradientTexture, pos).xy;

        if (length(gradient) < EPSILON) break;
        
        pos = pos + normalize(gradient) * texelSize * renderParams.x;
    }

    fragColor = vec4(pos, 1, 1);
}
