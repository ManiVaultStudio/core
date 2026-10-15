// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "HorizontalGroupAction.h"
#include "ColorAction.h"

namespace mv::gui {

/**
 * Palette color class
 *
 * For editing a palette color of the application
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT PaletteColorAction : public HorizontalGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct a palette action with a \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param colorGroup Color group of the palette color
     * @param colorRole Color role of the palette color
     */
    Q_INVOKABLE PaletteColorAction(QObject* parent, const QString& title, const QPalette::ColorGroup& colorGroup = QPalette::ColorGroup::Normal, const QPalette::ColorRole& colorRole = QPalette::ColorRole::Window);

    /**
     * Initialize the palette color action with a \p colorGroup and \p colorRole
     * @param colorGroup Color group of the palette color
     * @param colorRole Color role of the palette color
     */
    void initialize(const QPalette::ColorGroup& colorGroup, const QPalette::ColorRole& colorRole);

private:

    /** Update the color from the palette */
    void updateColorFromPalette();

private:
    QPalette::ColorGroup    _colorGroup;    /** Color group of the palette color */
    QPalette::ColorRole     _colorRole;     /** Color role of the palette color */
    ColorAction             _colorAction;   /** Palette color picker */
};

}

Q_DECLARE_METATYPE(mv::gui::PaletteColorAction)

inline const auto paletteColorActionMetaTypeId = qRegisterMetaType<mv::gui::PaletteColorAction*>("mv::gui::PaletteColorAction");
