// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "NumericalRangeAction.h"
#include "DecimalAction.h"

namespace hdps::gui {

/**
 * Decimal range action class
 *
 * Stores a decimal range and creates widgets to interact with it
 *
 * @author Thomas Kroes
 */
class DecimalRangeAction : public NumericalRangeAction<float, DecimalAction>
{
    Q_OBJECT

public:

    /** Widget class for decimal range action */
    class DecimalRangeWidget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param decimalRangeAction Pointer to decimal range action
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        DecimalRangeWidget(QWidget* parent, DecimalRangeAction* decimalRangeAction, const std::int32_t& widgetFlags = 0);

    protected:
        friend class DecimalRangeAction;
    };

protected:

    /**
     * Get widget representation of the decimal range action
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
    Q_INVOKABLE DecimalRangeAction(QObject* parent, const QString& title, const util::NumericalRange<float>& limits = util::NumericalRange<float>(INIT_LIMIT_MIN, INIT_LIMIT_MAX), const util::NumericalRange<float>& range = util::NumericalRange<float>(INIT_RANGE_MIN, INIT_RANGE_MAX), std::int32_t numberOfDecimals = INIT_NUMBER_OF_DECIMALS);
    
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
    void limitsChanged(const util::NumericalRange<float>& limits);

    /**
     * Signals that the range changed to \p range
     * @param range Range
     */
    void rangeChanged(const util::NumericalRange<float>& range);

protected:
    static constexpr float  INIT_LIMIT_MIN          = 0.0f;         /** Default minimum limit */
    static constexpr float  INIT_LIMIT_MAX          = 100.0f;       /** Default maximum limit */
    static constexpr float  INIT_RANGE_MIN          = 0.0f;         /** Default minimum range */
    static constexpr float  INIT_RANGE_MAX          = 100.0f;       /** Default maximum range */
    static constexpr float  INIT_NUMBER_OF_DECIMALS = 2;            /** Default number of decimals */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(hdps::gui::DecimalRangeAction)

inline const auto decimalRangeActionMetaTypeId = qRegisterMetaType<hdps::gui::DecimalRangeAction*>("hdps::gui::DecimalRangeAction");
