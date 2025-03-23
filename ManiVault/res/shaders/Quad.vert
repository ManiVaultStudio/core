// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#version 330 core

uniform mat4 	mvp;

out vec2 pass_texCoord;

void main() {
    pass_texCoord = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
    gl_Position = mvp * vec4(pass_texCoord * 2 - 1, 0, 1);
}
