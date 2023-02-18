#pragma once

#include "WidgetAction.h"

namespace hdps::gui {

/**
 * Horizontal group action class
 *
 * Group action which displays multiple child actions horizontally
 *
 * @author Thomas Kroes
 */
class HorizontalGroupAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Widget class for inline group action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param HorizontalGroupAction Pointer to inline group action
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        Widget(QWidget* parent, HorizontalGroupAction* HorizontalGroupAction, const std::int32_t& widgetFlags);

    protected:
        HorizontalGroupAction*   _HorizontalGroupAction;      /** Pointer to inline group action */

        friend class HorizontalGroupAction;
    };

protected:

    /**
     * Get widget representation of the inline group action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    }

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    HorizontalGroupAction(QObject* parent, const QString& title = "");

    /**
     * Get type string
     * @return Widget action type in string format
     */
    QString getTypeString() const override;

    /** Gets whether the group should show labels or not */
    bool getShowLabels() const;

    /**
     * Sets whether the group should show labels or not
     * @param showLabels Whether the group should show labels or not
     */
    void setShowLabels(bool showLabels);

    /**
     * Add \p action to the group
     * @param action Reference to action to add
     */
    void addAction(const WidgetAction& action);

    /**
     * Remove \p action from the group
     * @param action Reference to action to add
     */
    void removeAction(const WidgetAction& action);

    /**
     * Set actions to \p actions
     * @param actions Widget actions
     */
    void setActions(const ConstWidgetActions& actions);

    /**
     * Get actions
     * @return Widget actions
     */
    ConstWidgetActions getActions();

signals:

    /**
     * Signals that the actions changed to \p actions
     * @param actions Current actions in the group
     */
    void actionsChanged(const ConstWidgetActions& actions);

    /**
     * Signals that the group show labels option changed
     * @param showLabels Whether label are visible or not
     */
    void showLabelsChanged(const bool& showLabels);

private:
    ConstWidgetActions  _actions;       /** Widget actions in the group */
    bool                _showLabels;    /** Whether to show labels or not */
};

}
