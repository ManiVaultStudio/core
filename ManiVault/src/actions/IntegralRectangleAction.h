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
class CORE_EXPORT IntegralRectangleAction : public RectangleAction<QRect, IntegralRangeAction>
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param rectangle Rectangle
     */
    Q_INVOKABLE IntegralRectangleAction(QObject* parent, const QString& title, const QRect& rectangle = QRect());

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
     * @param rectangle Rectangle
     */
    void rectangleChanged(const QRectF& rectangle);

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::IntegralRectangleAction)

inline const auto integralRectangleActionMetaTypeId = qRegisterMetaType<mv::gui::IntegralRectangleAction*>("mv::gui::IntegralRectangleAction");
