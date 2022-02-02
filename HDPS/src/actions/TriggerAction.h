#pragma once

#include "WidgetAction.h"

#include <QPushButton>

namespace hdps {

namespace gui {

/**
 * Trigger action class
 *
 * Action class for trigger
 *
 * @author Thomas Kroes
 */
class TriggerAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget flags */
    enum WidgetFlag {

        /** Push button options */
        Icon            = 0x00001,          /** Enable push button icon */
        Text            = 0x00002,          /** Enable push button text */

        /** Push button configurations */
        IconText = Icon | Text              /** Push button with icon and text */
    };

public:

    /** Push button widget class for trigger action */
    class PushButtonWidget : public QPushButton {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param triggerAction Pointer to trigger action
         * @param widgetFlags Widget flags
         */
        PushButtonWidget(QWidget* parent, TriggerAction* triggerAction, const std::int32_t& widgetFlags);

    protected:
        TriggerAction*   _triggersAction;      /** Pointer to toggle action */

        friend class TriggerAction;
    };

protected:

    /**
     * Get widget representation of the trigger action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public: // Settings

    /**
     * Set value from variant
     * @param value Value
     */
    void setValueFromVariant(const QVariant& value) override final;

    /**
     * Convert value to variant
     * @return Value as variant
     */
    QVariant valueToVariant() const override final;

    /**
     * Convert default value to variant
     * @return Default value as variant
     */
    QVariant defaultValueToVariant() const override final;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    TriggerAction(QObject* parent, const QString& title = "");
};

}
}
