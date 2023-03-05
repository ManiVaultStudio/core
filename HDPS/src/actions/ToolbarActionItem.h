#pragma once

#include <QObject>

class QWidget;

namespace hdps::gui {

class WidgetAction;

/**
 * Toolbar action item class
 *
 * @author Thomas Kroes
 */
class ToolbarActionItem final : public QObject
{
    Q_OBJECT

public:
    
    /** State of the widget */
    enum class State {
        Collapsed,
        Expanded
    };

protected:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param horizontalToolbarAction Pointer to horizontal toolbar action that creates the widget
     * @param autoExpandPriority Priority with which action should be auto-expanded
     */
    ToolbarActionItem(QObject* parent, const WidgetAction* action, const State& state = State::Collapsed, std::int32_t autoExpandPriority = -1);

    /**
     * Get action
     * @return Pointer to action
     */
    const WidgetAction* getAction();

    /**
     * Get state of the action (expanded or collapsed)
     * @return State of the action
     */
    State getState() const;

    /**
     * Set state of the action to \p state
     * @param state State of the action (expanded or collapsed)
     */
    void setState(const State& state);

    /**
     * Get priority with which action should be auto-expanded
     * @return Priority with which action should be auto-expanded
     */
    std::int32_t getAutoExpandPriority() const;

    /**
     * Set priority with which action should be auto-expanded
     * @param autoExpandPriority Priority with which action should be auto-expanded
     */
    void setAutoExpandPriority(std::int32_t autoExpandPriority);

    /**
     * Create widget for item with \p parent
     * @param parent Pointer to parent
     * @return Pointer to created widget
     */
    QWidget* createWidget(QWidget* parent);

    /**
     * Respond to \p target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

signals:

    /**
     * Signals that the state changed to \p state
     * @param state Item state
     */
    void stateChanged(const State& state);

protected:
    const WidgetAction* _action;                /** Pointer to horizontal toolbar action that creates the widget */
    State               _state;                 /** Whether the item is expanded or collapsed */
    std::int32_t        _autoExpandPriority;    /** Priority with which action should be auto-expanded (higher priority w.r.t. other actions means it will auto-expanded sooner) */

    friend class ToolbarAction;
    friend class HorizontalToolbarAction;
    friend class ToolbarActionItemWidget;
};

}