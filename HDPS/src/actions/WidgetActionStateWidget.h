#pragma once

#include "WidgetActionWidget.h"

namespace hdps {

namespace gui {

class WidgetAction;

/**
 * Widget action state widget class
 *
 * Widget that is able to change state (expanded/collapsed)
 * 
 * @author Thomas Kroes
 */
class WidgetActionStateWidget : public QWidget
{
public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param widgetAction Pointer to the widget action that will be displayed
     * @param priority Lower priority increases the chance that the widget will be collapsed in a toolbar
     * @param state State of the widget
     */
    WidgetActionStateWidget(QWidget* parent, WidgetAction* widgetAction, const std::int32_t& priority = 0, const WidgetActionWidget::State& state = WidgetActionWidget::State::Collapsed);

    /** Get the current state */
    WidgetActionWidget::State getState() const;

    /**
     * Set the current state
     * @param state Current state
     */
    void setState(const WidgetActionWidget::State& state);

    /** Get the priority */
    std::int32_t getPriority() const;

    /**
     * Set the priority
     * @param priority Priority
     */
    void setPriority(const std::int32_t& priority);

    /**
     * Get the size hint for the state
     * @param state State of the widget
     * @return Size hint
     */
    QSize getSizeHint(const WidgetActionWidget::State& state) const;

private:
    WidgetAction*               _widgetAction;          /** Pointer to the widget action */
    WidgetActionWidget::State   _state;                 /** State of the widget */
    std::int32_t                _priority;              /** Priority of the widget */
    QWidget*                    _standardWidget;        /** Standard (expanded) widget */
    QWidget*                    _compactWidget;         /** Collapsed widget */
};

}
}
