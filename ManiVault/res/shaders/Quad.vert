// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#version 330 core

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 uv;

uniform mat4 	mvp;

out vec2 outUv;

void main() {
    gl_Position 	= mvp * vec4(vertex, 0, 1);
	
	outUv = (vertex / vec2(128.f, 128.f));
}
