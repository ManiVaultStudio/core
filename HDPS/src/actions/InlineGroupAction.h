#pragma once

#include "WidgetAction.h"

namespace hdps::gui {

/**
 * Inline group action class
 *
 * Group action which displays multiple child actions horizontally
 *
 * @author Thomas Kroes
 */
class InlineGroupAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Widget class for inline group action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param inlineGroupAction Pointer to inline group action
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        Widget(QWidget* parent, InlineGroupAction* inlineGroupAction, const std::int32_t& widgetFlags);

    protected:
        InlineGroupAction*   _inlineGroupAction;      /** Pointer to inline group action */

        friend class InlineGroupAction;
    };

protected:

    /**
     * Get widget representation of the inline group action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    }

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    InlineGroupAction(QObject* parent, const QString& title = "");

    /**
     * Get type string
     * @return Widget action type in string format
     */
    QString getTypeString() const override;
};

}
