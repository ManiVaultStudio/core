// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#version 330 core

uniform sampler2D tex;

in vec2 pass_texCoord;

out vec4 fragColor;

void main() {
    fragColor = texture(tex, pass_texCoord);
}
