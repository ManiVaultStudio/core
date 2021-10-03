#pragma once

#include <QWidget>

namespace hdps {

namespace gui {

class WidgetAction;

/**
 * Widget action widget class
 *
 * Base class for widgets that interact with a widget action
 * 
 * @author Thomas Kroes
 */
class WidgetActionWidget : public QWidget
{
public:

    /** Widget states */
    enum State {
        Standard,   /** Standard representation of the action (default) */
        Collapsed,  /** The widget is in a collapsed state and represented by a popup button */
        Popup       /** The widget representation of the action in a popup */
    };

public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param widgetAction Pointer to the widget action that will be displayed
     * @param state State of the widget
     */
    WidgetActionWidget(QWidget* parent, WidgetAction* widgetAction, const State& state);

protected: // Miscellaneous

    /**
     * Sets a popup layout
     * @param popupLayout Pointer to popup layout
     */
    void setPopupLayout(QLayout* popupLayout);

protected:
    WidgetAction*   _widgetAction;      /** Pointer to widget action that will be displayed */
    State           _state;             /** State of the widget */
};

}
}
