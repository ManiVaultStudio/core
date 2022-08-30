#pragma once

#include "WidgetActionWidget.h"

#include <QHBoxLayout>
#include <QToolButton>

namespace hdps {

namespace gui {

class WidgetAction;

/**
 * Widget action collapsed widget class
 *
 * Displays a tool button that shows the widget in a popup
 * 
 * @author Thomas Kroes
 */
class WidgetActionCollapsedWidget : public WidgetActionWidget
{
protected:

    /** Tool button class with custom paint behavior */
    class ToolButton : public QToolButton {
    public:

        /**
         * Paint event
         * @param paintEvent Pointer to paint event
         */
        void paintEvent(QPaintEvent* paintEvent);
    };

public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param widgetAction Pointer to the widget action that will be displayed in a popup
     */
    WidgetActionCollapsedWidget(QWidget* parent, WidgetAction* widgetAction);

    /**
     * Set the widget action
     * @param widgetAction Pointer to widget action
     */
    void setWidgetAction(WidgetAction* widgetAction) override;

    /** Get reference to the tool button */
    ToolButton& getToolButton() { return _toolButton; }

private:
    QHBoxLayout     _layout;            /** Layout */
    ToolButton      _toolButton;        /** Tool button for the popup */

    friend class WidgetAction;
};

}
}
