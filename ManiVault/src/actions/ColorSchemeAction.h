// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "HorizontalGroupAction.h"
#include "TriggerAction.h"
#include "EditColorSchemeAction.h"

namespace mv::gui {

/**
 * Color scheme action class
 *
 * For picking and editing custom color schemes
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ColorSchemeAction : public mv::gui::HorizontalGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    ColorSchemeAction(QObject* parent, const QString& title);

    /** Initialize the custom color scheme action */
    void initialize();

private:

    /** Invoked when the current custom color scheme changed */
    void currentColorSchemeChanged();

public: // Action getters

    OptionAction& getCurrentColorSchemeAction() { return _currentColorSchemeAction; }
    EditColorSchemeAction& getAddColorSchemeAction() { return _addColorSchemeAction; }
    TriggerAction& getRemoveColorSchemeAction() { return _removeColorSchemeAction; }
    EditColorSchemeAction& getEditColorSchemeAction() { return _editColorSchemeAction; }

private:
    OptionAction            _currentColorSchemeAction;      /** Current color scheme action  */
    EditColorSchemeAction   _addColorSchemeAction;          /** Add color scheme action */
    TriggerAction           _removeColorSchemeAction;       /** Remove color scheme action */
    EditColorSchemeAction   _editColorSchemeAction;         /** Edit color scheme action */
};

}