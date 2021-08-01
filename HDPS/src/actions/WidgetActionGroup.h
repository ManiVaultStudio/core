#pragma once

#include "WidgetAction.h"

namespace hdps {

namespace gui {

/**
 * Widget action group class
 *
 * Groups multiple actions
 * Instances of this class are represented by expandable/collapsible sections in the data editor widget
 * 
 * @author Thomas Kroes
 */
class WidgetActionGroup : public WidgetAction
{
    Q_OBJECT

public:
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

signals:

    /** Signals that the group got expanded */
    void expanded();

    /** Signals that the group got collapsed */
    void collapsed();

protected:
    bool    _expanded;      /** Whether or not the group is expanded */
};

}
}