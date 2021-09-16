#pragma once

#include <QToolButton>

namespace hdps {

namespace gui {

class WidgetAction;

/**
 * Widget action reset tool button class
 *
 * Button widget class for resetting a widget action
 * 
 * @author Thomas Kroes
 */
class WidgetActionResetButton : public QToolButton {
public:
    explicit WidgetActionResetButton(WidgetAction* widgetAction, QWidget* parent = nullptr);

protected:
    WidgetAction*   _widgetAction;          /** Pointer to widget action */
};

}
}
