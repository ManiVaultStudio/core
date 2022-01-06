#pragma once

#include "WidgetAction.h"

#include <QHBoxLayout>

namespace hdps {

namespace gui {

class ToolbarAction;

/**
 * Hidden items action class
 *
 * Shows hidden items in a popup
 *
 * @author Thomas Kroes
 */
class ToolbarHiddenItemsAction : public WidgetAction
{
protected:

    /** Widget for hidden items action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param toolbarHiddenItemsAction Pointer to toolbar hidden items action
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        Widget(QWidget* parent, ToolbarHiddenItemsAction* toolbarHiddenItemsAction, const std::int32_t& widgetFlags);

    protected:
        ToolbarHiddenItemsAction*   _hiddenItemsAction;     /** Pointer to hidden items action */
        QHBoxLayout                 _layout;                /** Main vertical layout */

        friend class ToolbarHiddenItemsAction;
    };

protected:

    /**
     * Get widget representation of the hidden items action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    };

protected:

    /**
     * Constructor
     * @param toolbarAction Reference to toolbar action
     */
    ToolbarHiddenItemsAction(ToolbarAction& toolbarAction);

    /**
     * Get toolbar action
     * @return Reference to toolbar action
     */
    ToolbarAction& getToolbarAction();

protected:
    ToolbarAction&    _toolbarAction;   /** Reference to toolbar action */

    friend class ToolbarWidget;
};

}
}
