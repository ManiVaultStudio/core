// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Badge.h"

#include <QPalette>

namespace mv::util
{

/** Styled icon modes */
enum class StyledIconMode
{
    ThemeAware,     /** Icon is colored by current theme with QPalette::ColorGroup and QPalette::ColorRole the (default) */
    FixedColor      /** Icon is colored by a fixed color */
};

struct StyledIconSettings
{
    StyledIconSettings() :
        _mode(StyledIconMode::ThemeAware),
        _colorGroupLightTheme(QPalette::ColorGroup::Normal),
        _colorGroupDarkTheme(QPalette::ColorGroup::Normal),
        _colorRoleLightTheme(QPalette::ColorRole::Text),
        _colorRoleDarkTheme(QPalette::ColorRole::Text)
    {
    }

    /**
     * Get color group for current theme
     * @return Color group for current theme
     */
    QPalette::ColorGroup getColorGroupForCurrentTheme() const;

    /**
     * Get color role for current theme
     * @return Color role for current theme
     */
    QPalette::ColorRole getColorRoleForCurrentTheme() const;

    QString                 _sha;                       /** Icon key */
    QString                 _modifierSha;               /** Modifier icon key */
    StyledIconMode          _mode;                      /** Styled icon coloring mode */
    QPalette::ColorGroup    _colorGroupLightTheme;      /** Color group for light theme */
    QPalette::ColorGroup    _colorGroupDarkTheme;       /** Color group for dark theme */
    QPalette::ColorRole     _colorRoleLightTheme;       /** Color role for light theme */
    QPalette::ColorRole     _colorRoleDarkTheme;        /** Color role for dark theme */
    QColor                  _fixedColor;                /** Fixed color */
    Badge::Parameters       _badgeParameters;           /** Badge parameters */
};

}
