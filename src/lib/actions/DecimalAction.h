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
     * @param logarithmic When true, the slider position is mapped to the value through a power curve so the low end of the range gets more slider travel (the value itself is unchanged)
     * @param logarithmicGamma Skew exponent for the logarithmic mapping; > 1 expands the low end of the range
     */
    Q_INVOKABLE explicit DecimalAction(QObject * parent, const QString& title, float minimum = INIT_MIN, float maximum = INIT_MAX, float value = INIT_VALUE, std::uint32_t numberOfDecimals = INIT_NUMBER_OF_DECIMALS, bool logarithmic = INIT_LOGARITHMIC, float logarithmicGamma = INIT_LOGARITHMIC_GAMMA);

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

    /** Gets whether the slider uses a logarithmic (power-curve) position mapping */
    bool isLogarithmic() const;

    /**
     * Sets whether the slider uses a logarithmic (power-curve) position mapping
     * @param logarithmic Whether to use the logarithmic mapping
     */
    void setLogarithmic(bool logarithmic);

    /** Gets the logarithmic skew exponent (gamma) */
    float getLogarithmicGamma() const;

    /**
     * Sets the logarithmic skew exponent (gamma); > 1 expands the low end of the range
     * @param logarithmicGamma Skew exponent
     */
    void setLogarithmicGamma(float logarithmicGamma);

    /**
     * Maps a normalized value in [0, 1] to a normalized slider position in [0, 1].
     * Identity when the scale is linear.
     * @param normalizedValue Normalized value
     * @return Normalized slider position
     */
    double valueToSliderPosition(double normalizedValue) const;

    /**
     * Maps a normalized slider position in [0, 1] to a normalized value in [0, 1].
     * Identity when the scale is linear.
     * @param normalizedSliderPosition Normalized slider position
     * @return Normalized value
     */
    double sliderPositionToValue(double normalizedSliderPosition) const;

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

    /** Signals that the logarithmic scale (enabled state or gamma) changed */
    void logarithmicScaleChanged();

protected:
    float   _singleStep;            /** Single step size for spin box */
    bool    _logarithmic;           /** Whether the slider maps its position through a power curve */
    float   _logarithmicGamma;      /** Skew exponent for the logarithmic mapping */

protected:
    static constexpr float  INIT_MIN                 = 0.0f;         /** Initialization minimum value */
    static constexpr float  INIT_MAX                 = 100.0f;       /** Initialization maximum value */
    static constexpr float  INIT_VALUE               = 0.0;          /** Initialization value */
    static constexpr int    INIT_DECIMALS            = 1;            /** Initialization number of decimals */
    static constexpr bool   INIT_LOGARITHMIC         = false;        /** Linear slider mapping by default */
    static constexpr float  INIT_LOGARITHMIC_GAMMA   = 3.0f;         /** Default logarithmic skew exponent */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::DecimalAction)

inline const auto decimalActionMetaTypeId = qRegisterMetaType<mv::gui::DecimalAction*>("mv::gui::DecimalAction");
