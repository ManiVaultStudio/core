#pragma once

#include "actions/TriggerAction.h"

#include <QMenu>

namespace hdps::gui {

class WidgetAction;

/**
 * Widget action context menu class
 * 
 * Context menu for one (or more) widget actions
 *
 * @author Thomas Kroes
 */
class WidgetActionContextMenu : public QMenu
{
public:

    /**
     * Construct with \p parent widget and \p widgetActions
     * @param parent Pointer to parent widget
     * @param actions Actions to create the context menu for
     */
    explicit WidgetActionContextMenu(QWidget* parent, WidgetActions actions);

private:
    WidgetActions   _actions;               /** Actions to create the context menu for */
    TriggerAction   _publishAction;         /** Publish action (so that other actions can connect) */
    TriggerAction   _disconnectAction;      /** Disconnect one or more parameters from shared parameter */
    TriggerAction   _removeAction;          /** Remove one or more shared parameters */
};

}
