// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "VerticalGroupAction.h"

namespace mv::gui {

/**
 * Palette class
 *
 * For editing the palette of the application
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT PaletteAction : public WidgetAction
{
    Q_OBJECT

public:

    /**
     * Construct a palette action with a \p parent object
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE PaletteAction(QObject* parent, const QString& title);

private:
    StringAction        _nameAction;    /** Name of the palette */

};

}

Q_DECLARE_METATYPE(mv::gui::PaletteAction)

inline const auto paletteActionMetaTypeId = qRegisterMetaType<mv::gui::PaletteAction*>("mv::gui::PaletteAction");
