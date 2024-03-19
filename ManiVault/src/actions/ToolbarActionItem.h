// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QObject>
#include <QSize>

class QWidget;

namespace mv::gui {

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
     * @param action Pointer to action that creates the widget
     * @param widgetFlags Flags to use in widget creation
     * @param state State of the widget
     * @param autoExpandPriority Priority with which action should be auto-expanded
     */
    ToolbarActionItem(QObject* parent, const WidgetAction* action, std::int32_t widgetFlags, const State& state = State::Collapsed, std::int32_t autoExpandPriority = -1);

    /**
     * Get action
     * @return Pointer to action
     */
    const WidgetAction* getAction();

    /**
     * Get widget flags
     * @return Flags to use in widget creation
     */
    std::int32_t getWidgetFlags();

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
     * Get widget size for \p state
     * @param state State to get the widget size for
     * @return Widget size
     */
    QSize getWidgetSize(const State& state) const;

    /**
     * Set widget \p size for \p state
     * @param state State to set the widget size for
     * @param size Widget size
     */
    void setWidgetSize(const QSize& size, const State& state);

    /**
     * Compare with \p other toolbar action item on the basis of their auto expand priority
     * @param other Other toolbar action item to compare with
     * @return Boolean determining whether the \p other toolbar action item is smaller
     */
    bool operator < (const ToolbarActionItem& other) const {
        return getAutoExpandPriority() < other.getAutoExpandPriority();
    }

    /**
     * Get whether the item is currently changing state
     * @return Boolean determining whether the item is currently changing state
     */
    bool isChangingState() const;

protected:

    /**
     * Set whether the item is currently changing state
     * @param changingState Boolean determining whether the item is currently changing state
     */
    void setChangingState(bool changingState);

signals:

    /**
     * Signals that the state changed to \p state
     * @param state Item state
     */
    void stateChanged(const State& state);

    /**
     * Signals that the changing state changed to \p changingState
     * @param changingState Boolean determining whether the item is currently changing state
     */
    void changingStateChanged(bool changingState);

    /**
     * Signals that the auto expand priority changed to \p autoExpandPriority
     * @param autoExpandPriority Auto expand priority
     */
    void autoExpandPriorityChanged(std::int32_t autoExpandPriority);

    /**
     * Signals that the size of widget with \p state changed to \p size
     * @param size New size of the widget
     * @param state State of the widget
     */
    void widgetSizeChanged(const QSize& size, const State& state);

protected:
    const WidgetAction* _action;                /** Pointer to horizontal toolbar action that creates the widget */
    std::int32_t        _widgetFlags;           /** Flags to use in widget creation */
    State               _state;                 /** Whether the item is expanded or collapsed */
    std::int32_t        _autoExpandPriority;    /** Priority with which action should be auto-expanded (higher priority w.r.t. other actions means it will auto-expanded sooner) */
    QSize               _widgetSizes[2];        /** Widget sizes in collapsed and expanded state respectively */
    bool                _changingState;         /** Boolean determining whether the item is currently changing state */

    friend class ToolbarAction;
    friend class HorizontalToolbarAction;
    friend class VerticalToolbarAction;
    friend class ToolbarActionItemWidget;
};

}
