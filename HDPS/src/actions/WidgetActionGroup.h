#pragma once

#include "WidgetAction.h"

class QWidget;
class QGridLayout;

namespace hdps {

namespace gui {

/**
 * Widget action group class
 *
 * Groups multiple actions
 * When exposed in a dataset, the widget is added to the data properties widget as a section
 * 
 * @author Thomas Kroes
 */
class WidgetActionGroup : public WidgetAction
{
    Q_OBJECT

public:

    /**
     * Group widget class for
     */
    class FormWidget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param widgetActionGroup Pointer to widget action group
         */
        FormWidget(QWidget* parent, WidgetActionGroup* widgetActionGroup);

        /**
         * Adds an action to the form
         * @param widgetAction Widget action
         */
        void addWidgetAction(WidgetAction& widgetAction, const bool& forceTogglePushButton = false);

        /** Get grid layout */
        QGridLayout* layout();

    protected:
        QGridLayout*    _layout;        /** Main grid layout */

        friend class WidgetActionGroup;
    };

public:
    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param expanded Whether the group is initially expanded/collapsed
     */
    explicit WidgetActionGroup(QObject* parent, const bool& expanded = false);

    /** Set expanded/collapsed */
    void setExpanded(const bool& expanded);

    /** Expand/collapse/toggle the group */
    void expand();
    void collapse();
    void toggle();

    /** Returns whether the group is expanded/collapsed */
    bool isExpanded() const;
    bool isCollapsed() const;

    /** Gets the group read-only */
    bool isReadOnly() const;

    /**
     * Sets the group read-only
     * @param readOnly Whether the group is read-only
     */
    void setReadOnly(const bool& readOnly);

signals:

    /** Signals that the group got expanded */
    void expanded();

    /** Signals that the group got collapsed */
    void collapsed();

    /** Signals that the group read-only status changed */
    void readOnlyChanged(const bool& readOnly);

protected:
    bool    _expanded;      /** Whether or not the group is expanded */
    bool    _readOnly;      /** Whether or not the group is read-only */
};

}
}
