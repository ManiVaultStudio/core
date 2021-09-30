#pragma once

#include "WidgetAction.h"

class QPushButton;

namespace hdps {

namespace gui {

/**
 * Trigger action class
 *
 * Check action with button UI
 *
 * @author Thomas Kroes
 */
class TriggerAction : public WidgetAction
{
    Q_OBJECT

public:

    /**
     * Push button widget class for trigger action
     */
    class PushButtonWidget : public WidgetActionWidget {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param triggerAction Pointer to trigger action
         */
        PushButtonWidget(QWidget* parent, TriggerAction* triggerAction);

    public:

        /** Gets the push button widget */
        QPushButton* getPushButton() { return _pushButton; }

    protected:
        QPushButton*    _pushButton;        /** Push button widget */

        friend class TriggerAction;
    };

protected:

    /**
     * Get widget representation of the trigger action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     * @param state State of the widget (for stateful widgets)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override;;

public:

    /** Reset to default */
    void reset() override {};

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
