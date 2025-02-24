// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::util
{

/** Styled icon modes */
enum class StyledIconMode
{
    ThemeAware,     /** Icon is colored by current theme with QPalette::ColorGroup and QPalette::ColorRole the (default) */
    FixedColor      /** Icon is colored by a fixed color */
};


}
