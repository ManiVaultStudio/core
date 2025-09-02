// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "NumericalAction.h"

#include <QDoubleSpinBox>
#include <QSlider>
#include <QLineEdit>

class QWidget;
class QPushButton;

namespace mv::gui {

/**
 * Decimal action class
 *
 * Stores a float value and creates widgets to interact with it
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT DecimalAction : public NumericalAction<float>
{
    Q_OBJECT

public:

    /** Spinbox widget class for decimal action */
    class CORE_EXPORT SpinBoxWidget : public QDoubleSpinBox
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param decimalAction Pointer to decimal action
         */
        SpinBoxWidget(QWidget* parent, DecimalAction* decimalAction);

        friend class DecimalAction;
    };

    /** Slider widget class for decimal action */
    class CORE_EXPORT SliderWidget : public QSlider
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param decimalAction Pointer to decimal action
         */
        SliderWidget(QWidget* parent, DecimalAction* decimalAction);

        friend class DecimalAction;
    };

    /** Line edit widget class for decimal action */
    class CORE_EXPORT LineEditWidget : public QLineEdit
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param decimalAction Pointer to decimal action
         */
        LineEditWidget(QWidget* parent, DecimalAction* decimalAction);

    private:
        QDoubleValidator    _validator;

        friend class DecimalAction;
    };

protected:

    /**
     * Get widget representation of the decimal action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param minimum Minimum value
     * @param maximum Maximum value
     * @param value Value
     * @param numberOfDecimals Number of decimals
     */
    Q_INVOKABLE explicit DecimalAction(QObject * parent, const QString& title, float minimum = INIT_MIN, float maximum = INIT_MAX, float value = INIT_VALUE, std::uint32_t numberOfDecimals = INIT_NUMBER_OF_DECIMALS);

    /**
     * Initialize the decimal action
     * @param minimum Minimum value
     * @param maximum Maximum value
     * @param value Value
     * @param numberOfDecimals Number of decimals
     */
    void initialize(float minimum, float maximum, float value, std::uint32_t numberOfDecimals = INIT_NUMBER_OF_DECIMALS);

    /** Gets the single step */
    float getSingleStep() const;

    /**
     * Sets the single step
     * @param singleStep Single step
     */
    void setSingleStep(float singleStep);

    /**
     * Get public copy of the action (other compatible actions can connect to it)
     * @return Pointer to public copy of the action
     */
    WidgetAction* getPublicCopy() const override;

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
     * Signals that the current value changed
     * @param value Current value that changed
     */
    void valueChanged(float value);

    /**
     * Signals that the minimum value changed
     * @param minimum New minimum
     */
    void minimumChanged(float minimum);

    /**
     * Signals that the maximum value changed
     * @param maximum New maximum
     */
    void maximumChanged(float maximum);

    /**
     * Signals that the prefix changed
     * @param prefix New prefix
     */
    void prefixChanged(const QString& prefix);

    /**
     * Signals that the suffix changed
     * @param suffix New suffix
     */
    void suffixChanged(const QString& suffix);

    /**
     * Signals that the number of decimals changed to \p numberOfDecimals
     * @param numberOfDecimals Number of decimals
     */
    void numberOfDecimalsChanged(std::uint32_t numberOfDecimals);

    /**
     * Signals that the single step changed
     * @param singleStep Single step
     */
    void singleStepChanged(float singleStep);

protected:
    float   _singleStep;    /** Single step size for spin box */

protected:
    static constexpr float  INIT_MIN        = 0.0f;         /** Initialization minimum value */
    static constexpr float  INIT_MAX        = 100.0f;       /** Initialization maximum value */
    static constexpr float  INIT_VALUE      = 0.0;          /** Initialization value */
    static constexpr int    INIT_DECIMALS   = 1;            /** Initialization number of decimals */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::DecimalAction)

inline const auto decimalActionMetaTypeId = qRegisterMetaType<mv::gui::DecimalAction*>("mv::gui::DecimalAction");
