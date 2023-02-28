#pragma once

#include "GroupAction.h"

namespace hdps::gui {

/**
 * Horizontal group action class
 *
 * Group action which displays multiple child actions horizontally
 *
 * @author Thomas Kroes
 */
class HorizontalGroupAction : public GroupAction
{
    Q_OBJECT

public:

    /** Widget class for horizontal group action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param horizontalGroupAction Pointer to horizontal group action
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        Widget(QWidget* parent, HorizontalGroupAction* horizontalGroupAction, const std::int32_t& widgetFlags);

    protected:
        HorizontalGroupAction*   _horizontalGroupAction;      /** Pointer to horizontal group action */

        friend class HorizontalGroupAction;
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
     * @param expanded Whether the group is initially expanded/collapsed
     */
    HorizontalGroupAction(QObject* parent, const QString& title, const bool& expanded = false);

    /**
     * Get type string
     * @return Widget action type in string format
     */
    QString getTypeString() const override;
};

}
