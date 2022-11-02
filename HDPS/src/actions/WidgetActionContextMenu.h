#pragma once

#include "actions/TriggerAction.h"

#include <QMenu>

namespace hdps {

namespace gui {

class WidgetAction;

/**
 * Widget action context menu class
 *
 * @author Thomas Kroes
 */
class WidgetActionContextMenu : public QMenu
{
public:

    /**
     * Constructor
     * @param widgetAction Pointer to widget action
     * @param parent Pointer to parent widget
     * @param windowFlags Window flags
     */
    explicit WidgetActionContextMenu(QWidget* parent, WidgetAction* widgetAction);

private:

    /** Populates the context menu with actions */
    void initialize();

private:
    WidgetAction*       _widgetAction;          /** Pointer to widget action */
    TriggerAction       _publishAction;         /** Publish action (so that other actions can connect) */
    TriggerAction       _disconnectAction;      /** Disconnect from public action */
};

}
}
