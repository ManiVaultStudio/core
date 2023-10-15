// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "NumericalRangeAction.h"
#include "IntegralAction.h"

namespace mv::gui {

/**
 * Integral range action class
 *
 * Stores an integral range and creates widgets to interact with it
 *
 * @author Thomas Kroes
 */
class IntegralRangeAction : public NumericalRangeAction<std::int32_t, IntegralAction>
{
    Q_OBJECT

public:

    /** Widget class for integral range action */
    class IntegralRangeWidget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param integralRangeAction Pointer to integral range action
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        IntegralRangeWidget(QWidget* parent, IntegralRangeAction* integralRangeAction, const std::int32_t& widgetFlags = 0);

    protected:
        friend class IntegralRangeAction;
    };

protected:

    /**
     * Get widget representation of the integral range action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param limitMin Range lower limit
     * @param limitMax Range upper limit
     * @param rangeMin Range minimum
     * @param rangeMax Range maximum
     */
    Q_INVOKABLE IntegralRangeAction(QObject* parent, const QString& title, const util::NumericalRange<std::int32_t>& limits = util::NumericalRange<std::int32_t>(INIT_LIMIT_MIN, INIT_LIMIT_MAX), const util::NumericalRange<std::int32_t>& range = util::NumericalRange<std::int32_t>(INIT_RANGE_MIN, INIT_RANGE_MAX));
    
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
     * Signals that the limits changed to \p limits
     * @param limits Limits
     */
    void limitsChanged(const util::NumericalRange<std::int32_t>& limits);

    /**
     * Signals that the range changed to \p range
     * @param range Range
     */
    void rangeChanged(const util::NumericalRange<std::int32_t>& range);

protected:
    static constexpr std::int32_t INIT_LIMIT_MIN    = 0;         /** Default minimum limit */
    static constexpr std::int32_t INIT_LIMIT_MAX    = 100;       /** Default maximum limit */
    static constexpr std::int32_t INIT_RANGE_MIN    = 0;         /** Default minimum range */
    static constexpr std::int32_t INIT_RANGE_MAX    = 100;       /** Default maximum range */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::IntegralRangeAction)

inline const auto integralRangeActionMetaTypeId = qRegisterMetaType<mv::gui::IntegralRangeAction*>("mv::gui::IntegralRangeAction");
