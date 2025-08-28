// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

namespace mv::gui {

/**
 * View plugin heads up display action class
 *
 * For displaying a heads-up display action in the view plugin (as an overlay action)
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ViewPluginHeadsUpDisplayAction : public WidgetAction
{
    Q_OBJECT
    
public:

    /**
     * Construct with pointer to \p parent object and \p title of the action
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE ViewPluginHeadsUpDisplayAction(QObject* parent, const QString& title);

public: // Serialization

    /**
     * Load image action from variant
     * @param variantMap Variant representation of the image action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save image action to variant
     * @return Variant map representation of the image action
     */
    QVariantMap toVariantMap() const override;
};

}

Q_DECLARE_METATYPE(mv::gui::ViewPluginHeadsUpDisplayAction)

inline const auto viewPluginHeadsUpDisplayActionMetaTypeId = qRegisterMetaType<mv::gui::ViewPluginHeadsUpDisplayAction*>("mv::gui::ViewPluginHeadsUpDisplayAction");
