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
public:
    explicit WidgetActionGroup(QObject* parent, const bool& expanded = false);

    /** Returns whether or not the group is expanded */
    bool isExpanded() const;

protected:
    bool    _expanded;      /** Whether or not the group is expanded */
};

}
}