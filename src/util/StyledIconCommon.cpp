// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StyledIconCommon.h"

#include "CoreInterface.h"

namespace mv::util
{

QPalette::ColorGroup StyledIconSettings::getColorGroupForCurrentTheme() const
{
    return mv::theme().isSystemDarkColorSchemeActive() ? _colorGroupDarkTheme : _colorGroupLightTheme;
}

QPalette::ColorRole StyledIconSettings::getColorRoleForCurrentTheme() const
{
    return mv::theme().isSystemDarkColorSchemeActive() ? _colorRoleDarkTheme : _colorRoleLightTheme;
}

}
