// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "RectangleAction.h"
#include "DecimalRangeAction.h"

#include <QRectF>

namespace mv::gui {

/**
 * Decimal rectangle action class
 *
 * Stores a decimal rectangle and creates interfaces to interact with it
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT DecimalRectangleAction : public RectangleAction<DecimalRangeAction>
{
    Q_OBJECT

public:

    using RectangleAction<DecimalRangeAction>::setRectangle;

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE DecimalRectangleAction(QObject* parent, const QString& title);

    /**
     * Set rectangle to \p rectangle
     * @param rectangle Rectangle
     */
    void setRectangle(const QRectF& rectangle)
    {
        RectangleAction::setRectangle(rectangle.left(), rectangle.right(), rectangle.bottom(), rectangle.top());
    }

    /**
     * Get rectangle as Qt rectangle
     * @return Qt Rectangle
     */
    QRectF toRectF() const;

    /**
     * Set rectangle from Qt \p rectangle
     * @param rectangle Qt Rectangle
     */
    void setRectF(const QRectF& rectangle);

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
    void rectangleChanged(float left, float right, float bottom, float top);

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::DecimalRectangleAction)

inline const auto decimalRectangleActionMetaTypeId = qRegisterMetaType<mv::gui::DecimalRectangleAction*>("mv::gui::DecimalRectangleAction");
