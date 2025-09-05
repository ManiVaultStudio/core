// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "NumericalAction.h"

#include <QSpinBox>
#include <QSlider>
#include <QLineEdit>
#include <QIntValidator>

class QWidget;
class QPushButton;

namespace mv::gui {

/**
 * Integral widget action class
 *
 * Stores an integral value and creates widgets to interact with it
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT IntegralAction : public NumericalAction<std::int32_t>
{
    Q_OBJECT

public:

    /** Spinbox widget class for integral action */
    class CORE_EXPORT SpinBoxWidget : public QSpinBox
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param integralAction Pointer to integral action
         */
        SpinBoxWidget(QWidget* parent, IntegralAction* integralAction);
        
        friend class IntegralAction;
    };

    /** Slider widget class for integral action */
    class CORE_EXPORT SliderWidget : public QSlider
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param integralAction Pointer to integral action
         */
        SliderWidget(QWidget* parent, IntegralAction* integralAction);

        friend class IntegralAction;
    };

    /** Line edit widget class for integral action */
    class CORE_EXPORT LineEditWidget : public QLineEdit
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param integralAction Pointer to integral action
         */
        LineEditWidget(QWidget* parent, IntegralAction* integralAction);

    private:
        QIntValidator   _validator;

        friend class IntegralAction;
    };

protected:

    /**
     * Get widget representation of the integral action
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
     */
    Q_INVOKABLE explicit IntegralAction(QObject * parent, const QString& title, std::int32_t minimum = INIT_MIN, std::int32_t maximum = INIT_MAX, std::int32_t value = INIT_VALUE);

    /**
     * Initialize the integral action
     * @param minimum Minimum value
     * @param maximum Maximum value
     * @param value Value
     */
    void initialize(std::int32_t minimum, std::int32_t maximum, std::int32_t value);

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
     * Load widget action from variant
     * @param Variant representation of the widget action
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
    void valueChanged(std::int32_t value);

    /**
     * Signals that the default value changed
     * @param defaultValue Default value that changed
     */
    void defaultValueChanged(std::int32_t defaultValue);

    /**
     * Signals that the minimum value changed
     * @param minimum New minimum
     */
    void minimumChanged(std::int32_t minimum);

    /**
     * Signals that the maximum value changed
     * @param maximum New maximum
     */
    void maximumChanged(std::int32_t maximum);

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

protected:
    static constexpr std::int32_t INIT_MIN      = 0;        /** Initialization minimum value */
    static constexpr std::int32_t INIT_MAX      = 100;      /** Initialization maximum value */
    static constexpr std::int32_t INIT_VALUE    = 0;        /** Initialization value */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::IntegralAction)

inline const auto integralActionMetaTypeId = qRegisterMetaType<mv::gui::IntegralAction*>("mv::gui::IntegralAction");
