// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#version 330 core

uniform sampler2D tex;
uniform float norm;

in vec2 pass_texCoord;

out vec4 fragColor;

void main() {
    float f = 1 - (texture(tex, pass_texCoord).r * norm);
    fragColor = vec4(vec3(f), 1);
	
	if (pass_texCoord.x < 0.1 || pass_texCoord.x > 0.9)
		fragColor = vec4(1,0,0,1);
}
