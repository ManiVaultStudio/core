// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "VerticalGroupAction.h"
#include "TriggerAction.h"
#include "PaletteAction.h"

namespace mv::gui {

/**
 * Edit color scheme action class
 *
 * For creating and editing color schemes
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT EditColorSchemeAction : public VerticalGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    EditColorSchemeAction(QObject* parent, const QString& title);

    /**
     * Initialize the edit color scheme action
     * @param add Add flag
     */
    void initialize(bool add = false);

public: // Action getters

    StringAction& getNameAction() { return _nameAction; }
    PaletteAction& getPaletteAction() { return _paletteAction; }

private:
    StringAction    _nameAction;        /** Name action */
    PaletteAction   _paletteAction;     /** Palette action */
    TriggerAction   _updateAction;      /** Update action */
};

}