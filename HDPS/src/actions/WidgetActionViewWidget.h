#pragma once

#include <QWidget>

namespace hdps::gui {

class WidgetAction;
class WidgetActionHighlightWidget;

/**
 * Widget action view widget class
 *
 * Base class for widgets that provide a view on an action.
 * 
 * @author Thomas Kroes
 */
class WidgetActionViewWidget : public QWidget
{
public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param action Pointer to the  action that will be displayed
     */
    WidgetActionViewWidget(QWidget* parent, WidgetAction* action);

    /**
     * Get the source action
     * @return Pointer to source action
     */
    virtual WidgetAction* getAction() final;

    /**
     * Set the source action
     * @param widgetAction Pointer to source action
     */
    virtual void setAction(WidgetAction* widgetAction);

private:
    WidgetAction*                   _action;            /** Pointer to action that will be displayed */
    WidgetActionHighlightWidget*    _highlightWidget;   /** Pointer to highlight widget */
};

}
