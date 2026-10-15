// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "TriggerAction.h"

namespace mv::gui {

/**
 * Icon action class (WIP)
 *
 * For icon storage
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT IconAction : public TriggerAction
{
    Q_OBJECT
    
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE IconAction(QObject* parent, const QString& title);

    /**
     * Set the current icon from \p image
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
};

}

Q_DECLARE_METATYPE(mv::gui::IconAction)

inline const auto iconActionMetaTypeId = qRegisterMetaType<mv::gui::IconAction*>("mv::gui::IconAction");
