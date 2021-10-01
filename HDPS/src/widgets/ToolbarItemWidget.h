#pragma once

#include "actions/WidgetActionWidget.h"

#include "CollapsedWidget.h"

#include <QWidget>

namespace hdps {

namespace gui {

/**
 * Widget action state widget class
 *
 * Widget that is able to change state (expanded/collapsed)
 * 
 * @author Thomas Kroes
 */
class ToolbarItemWidget : public QWidget
{
public:
    using GetWidgetFN = std::function<QSharedPointer<QWidget>(const WidgetActionWidget::State& state)>;

public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param getWidget Function for getting the widget
     * @param priority Lower priority increases the chance that the widget will be collapsed in a toolbar
     * @param state State of the widget
     */
    ToolbarItemWidget(QWidget* parent, GetWidgetFN getWidget, const std::int32_t& priority = 0, const WidgetActionWidget::State& state = WidgetActionWidget::State::Collapsed);

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
    WidgetActionWidget::State   _state;                 /** State of the widget */
    std::int32_t                _priority;              /** Priority of the widget */
    GetWidgetFN                 _getWidget;             /** Get standard widget */
    QHBoxLayout                 _layout;                /** Main horizontal layout */
    CollapsedWidget             _collapsedWidget;       /** Collapsed item tool button widget */
};

}
}
