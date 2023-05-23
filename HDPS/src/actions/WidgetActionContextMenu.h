#pragma once

#include "actions/TriggerAction.h"

#include <QMenu>
#include <QDialog>

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

protected:

    /** Dialog class for editing an action */
    class EditActionDialog : public QDialog
    {
    public:

        /**
         * Construct with \p parent widget and edit \p action
         * @param parent Pointer to parent widget (if any)
         * @param action Reference to edit action
         */
        EditActionDialog(QWidget* parent, WidgetAction& action);

    private:
        WidgetAction&   _action;      /** Reference of widget action to edit */
    };

private:
    WidgetActions   _actions;               /** Actions to create the context menu for */
    TriggerAction   _publishAction;         /** Publish action (so that other actions can connect) */
    TriggerAction   _disconnectAction;      /** Disconnect one or more parameters from shared parameter */
    TriggerAction   _disconnectAllAction;   /** Disconnect all parameters which are connected to the shared parameter */
    TriggerAction   _removeAction;          /** Remove one or more shared parameters */
    TriggerAction   _editAction;            /** Action for triggering a public action edit */
};

}
