// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "RectangleAction.h"
#include "IntegralRangeAction.h"

namespace mv::gui {

/**
 * Integral rectangle action class
 *
 * Stores an integral rectangle and creates interfaces to interact with it
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT IntegralRectangleAction : public RectangleAction<IntegralRangeAction>
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE IntegralRectangleAction(QObject* parent, const QString& title);

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

signals:

    /**
     * Signals that the rectangle changed
     * @param left Rectangle left
     * @param right Rectangle right
     * @param bottom Rectangle bottom
     * @param top Rectangle top
     */
    void rectangleChanged(std::int32_t left, std::int32_t right, std::int32_t bottom, std::int32_t top);

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::IntegralRectangleAction)

inline const auto integralRectangleActionMetaTypeId = qRegisterMetaType<mv::gui::IntegralRectangleAction*>("mv::gui::IntegralRectangleAction");
