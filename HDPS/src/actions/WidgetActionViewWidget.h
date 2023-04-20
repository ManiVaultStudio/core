#pragma once

#include "widgets/OverlayWidget.h"

#include <QWidget>

namespace hdps::gui {

class WidgetAction;

/**
 * Widget action view widget class
 *
 * Base class for widgets that provide a view on an action.
 * 
 * This class provides basic functionality for:
 * - Storing and retrieving the action
 * - Highlighting
 * 
 * @author Thomas Kroes
 */
class WidgetActionViewWidget : public QWidget
{
protected:

    /** Widget class for highlighting an action view widget */
    class HighlightWidget final : public OverlayWidget {
    public:

        /**
         * Construct with parent widget
         * @param parent Pointer to parent widget
         */
        HighlightWidget(QWidget* parent);
    };

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
    WidgetAction*       _action;            /** Pointer to action that will be displayed */
    HighlightWidget*    _highlightWidget;   /** Pointer to highlight widget (if any) */
};

}
