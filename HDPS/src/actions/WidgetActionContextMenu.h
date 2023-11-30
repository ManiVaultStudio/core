// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/TriggerAction.h"
#include "actions/StringsAction.h"
#include "actions/VerticalGroupAction.h"

#include <QMenu>
#include <QDialog>

namespace mv::gui {

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

    /** Dialog class for asking permission prior to removing a public action */
    class ConfirmRemovePublicActionDialog : public QDialog
    {
    public:

        /**
         * Construct with \p parent widget and edit \p publicAction
         * @param parent Pointer to parent widget (if any)
         * @param publicAction Reference to public action
         */
        ConfirmRemovePublicActionDialog(QWidget* parent, WidgetActions publicActions);

    private:
        WidgetActions   _publicActions;     /** Public action which is about to be removed */
        StringsAction   _namesAction;       /** Action which lists the parameter names which are about to be removed */
        TriggerAction   _removeAction;      /** Accept when triggered */
        TriggerAction   _cancelAction;      /** Reject when triggered */
    };

    /** Dialog class for editing one ore more actions */
    class EditActionsDialog : public QDialog
    {
    public:

        /**
         * Construct with \p parent widget and edit widget \p actions
         * @param parent Pointer to parent widget (if any)
         * @param action Widget actions to edit
         */
        EditActionsDialog(QWidget* parent, WidgetActions actions);

        /**
         * Invoked when the dialog is closed
         * @param event Pointer to the close event
         */
        void closeEvent(QCloseEvent* event);

    private:
        WidgetActions           _actions;               /** Widget actions to edit */
        VerticalGroupAction     _actionsGroupAction;    /** Vertical group action */
    };

private:
    WidgetActions   _actions;               /** Actions to create the context menu for */
    TriggerAction   _publishAction;         /** Publish action (so that other actions can connect) */
    TriggerAction   _connectAction;         /** Action which triggers a visual connection process */
    TriggerAction   _disconnectAction;      /** Disconnect one or more parameters from shared parameter */
    TriggerAction   _disconnectAllAction;   /** Disconnect all parameters which are connected to the shared parameter */
    TriggerAction   _removeAction;          /** Remove one or more shared parameters */
    TriggerAction   _editAction;            /** Action for triggering a public action edit */
};

}
