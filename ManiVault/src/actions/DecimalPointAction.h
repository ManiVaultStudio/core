// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "NumericalPointAction.h"

namespace mv::gui {

/**
 * Decimal action class
 *
 * Stores a two-dimensional decimal point value.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT DecimalPointAction : public NumericalPointAction<float, DecimalAction>
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE explicit DecimalPointAction(QObject* parent, const QString& title);

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

signals:

    /**
     * Signals that the coordinate values changed to \p x and \p y
     * @param x X coordinate value
     * @param y Y coordinate value
     */
    void valueChanged(float x, float y);

protected:
    static constexpr float  INIT_VALUE      = 0.0;      /** Initialization value */
    static constexpr int    INIT_DECIMALS   = 1;        /** Initialization number of decimals */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::DecimalPointAction)

inline const auto decimalPointActionMetaTypeId = qRegisterMetaType<mv::gui::DecimalPointAction*>("mv::gui::DecimalPointAction");
