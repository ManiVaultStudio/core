// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/HorizontalGroupAction.h"
#include "actions/FilePickerAction.h"
#include "actions/IconAction.h"
#include "actions/TriggerAction.h"

namespace mv::gui {

/**
 * Icon picker action class (WIP)
 *
 * For picking and displaying an icon
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT IconPickerAction : public HorizontalGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE IconPickerAction(QObject* parent, const QString& title);

    /**
     * Get the current icon
     * Facade for IconAction::getIcon()
     * @return Icon
     */
    const util::StyledIcon& getIcon() const override;

    /**
     * Set the current icon to \p icon
     * Facade for IconAction::setIcon(...)
     * @param icon Current icon
     */
    void setIcon(const util::StyledIcon& icon);

    /**
     * Set the current icon from \p image
     * Facade for IconAction::setIconFromImage(...)
     * @param image Image to convert to icon
     */
    void setIconFromImage(const QImage& image);

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

    FilePickerAction& getInputFilePathPickerAction() { return _inputFilePathPickerAction; }
    IconAction& getIconAction() { return _iconAction; }

signals:

    /**
     * Signals that the current icon changed to \p icon
     * @param icon Current icon that changed
     */
    void iconChanged(const QIcon& icon);

private:
    FilePickerAction    _inputFilePathPickerAction;     /** Action for picking an input PNG file which serve as input for the internal icon (QIcon) */
    IconAction          _iconAction;                    /** Action which holds the current icon */
};

}

Q_DECLARE_METATYPE(mv::gui::IconPickerAction)

inline const auto iconPickerActionMetaTypeId = qRegisterMetaType<mv::gui::IconPickerAction*>("mv::gui::IconPickerAction");
