// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#version 330 core

uniform vec2 start;
uniform vec2 end;

void main()
{
    vec2 vertex;

    switch (gl_VertexID) {
    case 0: vertex = vec2(start.x, start.y); break;
    case 1: vertex = vec2(end.x, start.y); break;
    case 2: vertex = vec2(start.x, end.y); break;
    case 3: vertex = vec2(end.x, end.y); break;
    }

    gl_Position = vec4(vertex, 0, 1);
}
