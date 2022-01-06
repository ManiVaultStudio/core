#pragma once

#include "ToolbarItemWidget.h"

#include <QHBoxLayout>
#include <QVariantAnimation>

namespace hdps {

namespace gui {

class WidgetAction;

/**
 * Stateful item class
 *
 * Selectively shows either a standard or collapsed widget of an action
 *
 * @author Thomas Kroes
 */
class ToolbarActionWidget : public ToolbarItemWidget
{

public:

    /** Toolbar item widget states */
    enum State {
        Undefined,      /** Item state is not defined */
        Collapsed,      /** Item is in a collapsed state (accessible through a tool button) */
        Standard,       /** Item is in a standard state */
        Hidden          /** Item is hidden */
    };

public:

    /**
     * Constructor
     * @param toolbarWidget Pointer to toolbar widget
     * @param index Index
     * @param action Reference to action
     * @param action Reference to action
     */
    ToolbarActionWidget(ToolbarWidget* toolbarWidget, WidgetAction& action, std::int32_t priority);

public:

    /**
     * Invoked when the state changed
     * @return previousState Previous state of the item widget
     * @return currentState Current state of the item widget
     */
    void stateChanged(std::int32_t previousState, std::int32_t currentState) override;

    /**
     * Get widget
     * @param itemState State for which to fetch the widget
     * @return Pointer to stateful item widget
     */
    QWidget* getWidget(const std::int32_t& state);

    /**
     * Get width
     * @return Width of the stateful item widget
     */
    std::int32_t getWidth() const override;

    /**
     * Get width
     * @param state State for which to fetch the width
     * @return Width
     */
    std::int32_t getWidth(const std::int32_t& state) const override;

public:

    /**
     * Get priority
     * @return Priority
     */
    std::int32_t getPriority() const;

    /**
     * Respond to target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

    /**
     * Smaller than operator
     * @param other Stateful item to compare with
     * @return Whether the other item is smaller than ours (in terms of the visibility priority)
     */
    bool operator<(ToolbarActionWidget other) {
        return getPriority() < other.getPriority();
    }

protected:
    ToolbarWidget*         _toolbarWidget;          /** Pointer to owning toolbar widget */
    WidgetAction&          _action;                 /** Reference to action */
    std::int32_t           _priority;               /** Visibility priority */
    QHBoxLayout            _layout;                 /** Widget layout */
    FadeableWidget         _collapsedWidget;        /** Fadeable collapsed widget */
    FadeableWidget         _standardWidget;         /** Fadeable standard widget */
    QVariantAnimation      _sizeAnimation;          /** Animation to control the size of the widget */
};

}
}
