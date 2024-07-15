// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/IconPickerAction.h"

#include <QTimer>

namespace mv::gui {

/**
 * Application icon picker action class
 *
 * Extends the IconPickerAction class with testing functionality
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ApplicationIconPickerAction : public IconPickerAction
{
public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE ApplicationIconPickerAction(QObject* parent, const QString& title);

    /** Use IconPickerAction basic functions */
    using IconPickerAction::getIcon;
    using IconPickerAction::setIcon;
    using IconPickerAction::setIconFromImage;

public: // Serialization

    /**
     * Load image action from variant
     * @param Variant representation of the image action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save image action to variant
     * @return Variant representation of the image action
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    TriggerAction& getTestAction() { return _testAction; }

signals:

    /**
     * Signals that the current icon changed to \p icon
     * @param icon Current icon that changed
     */
    void iconChanged(const QIcon& icon);

private:
    TriggerAction   _testAction;    /** Trigger an icon test */
    QTimer          _testTimer;     /** Interval in which to set the application icon */
};

}

Q_DECLARE_METATYPE(mv::gui::ApplicationIconPickerAction)

inline const auto applicationIconPickerActionMetaTypeId = qRegisterMetaType<mv::gui::ApplicationIconPickerAction*>("mv::gui::ApplicationIconPickerAction");
