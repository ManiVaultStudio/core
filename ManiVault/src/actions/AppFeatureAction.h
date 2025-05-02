// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "HorizontalGroupAction.h"
#include "ToggleAction.h"
#include "VerticalGroupAction.h"

namespace mv::gui {

/**
 * App feature action class
 *
 * Base vertical group action class for grouping app feature settings
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AppFeatureAction : public HorizontalGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE AppFeatureAction(QObject* parent, const QString& title);

    /**
     * Add \p action to the group
     * @param action Pointer to action to add
     * @param widgetFlags Action widget flags (default flags if -1)
     * @param widgetConfigurationFunction When set, overrides the standard widget configuration function in the widget action
     * @param load Currently not used
     */
    void addAction(WidgetAction* action, std::int32_t widgetFlags = -1, WidgetConfigurationFunction widgetConfigurationFunction = WidgetConfigurationFunction(), bool load = true) override;

public: // Action getters

    ToggleAction& getEnabledAction() { return _enabledAction; }

private:
    ToggleAction            _enabledAction;     /** Settings action for the app feature */ 
    VerticalGroupAction     _settingsAction;    /** Vertical group action for settings */
};

}

Q_DECLARE_METATYPE(mv::gui::AppFeatureAction)

inline const auto appFeatureActionMetaTypeId = qRegisterMetaType<mv::gui::AppFeatureAction*>("mv::gui::AppFeatureAction");
