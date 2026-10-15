// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

#include <QGridLayout>

namespace mv::gui {

/**
 * Stretch action class
 *
 * Action class for adding layout stretch to group actions (does not have a widget)
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT StretchAction : public WidgetAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Group title
     * @param stretch Layout stretch
     */
    Q_INVOKABLE StretchAction(QObject* parent, const QString& title, std::int32_t stretch = 1);

protected: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     * @param recursive Whether to also connect descendant child actions
     */
    void connectToPublicAction(WidgetAction* publicAction, bool recursive) override;

    /**
     * Disconnect this action from its public action
     * @param recursive Whether to also disconnect descendant child actions
     */
    void disconnectFromPublicAction(bool recursive) override;

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;
};

}

Q_DECLARE_METATYPE(mv::gui::StretchAction)

inline const auto stretchActionMetaTypeId = qRegisterMetaType<mv::gui::StretchAction*>("mv::gui::StretchAction");