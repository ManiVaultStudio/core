// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/HorizontalGroupAction.h"
#include "actions/IconPickerAction.h"
#include "actions/ToggleAction.h"
#include "actions/TriggerAction.h"

#include <QTimer>

namespace mv::gui {

/**
 * Application icon action class
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ApplicationIconAction : public HorizontalGroupAction
{
    Q_OBJECT
    
public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE ApplicationIconAction(QObject* parent, const QString& title);

    /** Set the main window icon to the override application icon */
    void overrideMainWindowIcon() const;

    /** Set the application icon to the default */
    static void resetApplicationIcon();

public: // Serialization

    /**
     * Load image action from variant
     * @param variantMap Variant map representation of the image action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save image action to variant
     * @return Variant representation of the image action
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    ToggleAction& getOverrideAction() { return _overrideAction; }
    IconPickerAction& getIconPickerAction() { return _iconPickerAction; }
    TriggerAction& getTestAction() { return _testAction; }

signals:

    /**
     * Signals that the current icon changed to \p icon
     * @param icon Current icon that changed
     */
    void iconChanged(const QIcon& icon);

private:
    ToggleAction        _overrideAction;    /** Toggles override application icon action when loading a published project at ManiVault startup */
    IconPickerAction    _iconPickerAction;  /** Application icon picker action */
    TriggerAction       _testAction;        /** Trigger an icon test */
    QTimer              _testTimer;         /** Interval in which to set the application icon */
};

}

Q_DECLARE_METATYPE(mv::gui::ApplicationIconAction)

inline const auto applicationIconActionMetaTypeId = qRegisterMetaType<mv::gui::ApplicationIconAction*>("mv::gui::ApplicationIconAction");
