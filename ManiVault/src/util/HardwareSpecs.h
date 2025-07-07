// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "HardwareSpec.h"

#include <QString>

namespace mv::util
{

CORE_EXPORT QString loadFileContents(const QString& path);

CORE_EXPORT class HardwareSpecs
{
private:
    DisplayResolution   _displayResolution;     /** Display resolution of the system */
};

}
