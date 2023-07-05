// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#version 330 core

uniform float sigma;

uniform mat3 projMatrix;

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec2 position;

out vec2 pass_texCoord;

void main() {
    pass_texCoord = texCoord;
    vec2 pos = (projMatrix * vec3(position, 1)).xy;
    gl_Position = vec4(vertex * sigma + pos, 0, 1);
}
