// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#version 330 core

uniform sampler2D tex;
uniform float norm;

in vec2 outUv;

out vec4 fragColor;

void main() {
    float f = 1 - (texture(tex, outUv).r * norm);
    fragColor = vec4(vec3(f), 1);
}