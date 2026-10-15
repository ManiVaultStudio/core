// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#version 330 core

uniform sampler2D densityTexture;

uniform vec4 renderParams;

in vec2 pass_texCoord;

out vec4 fragColor;

void main()
{
    vec3 gradient;
    
    float density = texture(densityTexture, pass_texCoord.xy).r;
    if(density < renderParams.y)
    {
        gradient = vec3(0.0);
    }
    else
    {
	    vec3 texelSize = vec3( renderParams.zz, 0.0);

        vec4 neighborDensities;
        neighborDensities.x = texture(densityTexture, pass_texCoord.xy + texelSize.xz).r;
        neighborDensities.y = texture(densityTexture, pass_texCoord.xy - texelSize.xz).r;
        neighborDensities.z = texture(densityTexture, pass_texCoord.xy + texelSize.zy).r;
        neighborDensities.w = texture(densityTexture, pass_texCoord.xy - texelSize.zy).r;
        // this does not work as desired when density and gradient textures are of different sizes.
        // I.e., it is a 1 texel step in the input texture when it is desired in the output texture.
        //neighborDensities.x = textureOffset(densityTexture, pass_texCoord, ivec2(1, 0)).r;
        //neighborDensities.y = textureOffset(densityTexture, pass_texCoord, ivec2(-1, 0)).r;
        //neighborDensities.z = textureOffset(densityTexture, pass_texCoord, ivec2(0, 1)).r;
        //neighborDensities.w = textureOffset(densityTexture, pass_texCoord, ivec2(0, -1)).r;
        neighborDensities *= renderParams.x;

        gradient = vec3(neighborDensities.x - neighborDensities.y, neighborDensities.z - neighborDensities.w, density);
    }
    
    fragColor = vec4(gradient, 1.0);
}
