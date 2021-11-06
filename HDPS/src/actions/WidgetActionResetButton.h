#pragma once

#include <QToolButton>

namespace hdps {

namespace gui {

class WidgetAction;

/**
 * Widget action reset tool button class
 *
 * Tool button widget class for resetting a widget action
 * 
 * @author Thomas Kroes
 */
class WidgetActionResetButton : public QToolButton {
public:

    /**
     * Constructor
     * @param widgetAction Pointer to widget action of which to reset
     * @param parent Pointer to parent widget
     */
    explicit WidgetActionResetButton(WidgetAction* widgetAction, QWidget* parent = nullptr);

protected:
    WidgetAction*   _widgetAction;          /** Pointer to widget action */
};

}
}
