// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Math.h"

namespace hdps
{
namespace util
{

    float lerp(float v0, float v1, float t)
    {
        return (1 - t) * v0 + t * v1;
    }

} // namespace util

} // namespace hdps
