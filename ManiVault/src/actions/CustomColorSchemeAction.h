// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "HorizontalGroupAction.h"
#include "TriggerAction.h"

namespace mv::gui {

/**
 * Custom color scheme action class
 *
 * For picking and editing custom color schemes
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT CustomColorSchemeAction : public mv::gui::HorizontalGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    CustomColorSchemeAction(QObject* parent, const QString& title);

    /** Initialize the custom color scheme action */
    void initialize();

private:

    /** Invoked when the current custom color scheme changed */
    void currentCustomColorSchemeChanged();

public: // Action getters

    OptionAction& getCurrentColorSchemeAction() { return _currentColorSchemeAction; }
    TriggerAction& getAddColorSchemeAction() { return _addColorSchemeAction; }
    TriggerAction& getEditColorSchemeAction() { return _editColorSchemeAction; }

private:
    OptionAction    _currentColorSchemeAction;          /** Custom color scheme action  */
    TriggerAction   _addColorSchemeAction;              /** Add custom color scheme action */
    TriggerAction   _editColorSchemeAction;             /** Edit custom color scheme action */
};

}