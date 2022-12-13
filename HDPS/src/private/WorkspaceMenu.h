#pragma once

#include <actions/TriggerAction.h>

#include <QMenu>
#include <QIcon>

/**
 * Work space menu class
 * 
 * Menu class for loading/saving workspaces.
 * 
 * @author Thomas Kroes
 */
class WorkspaceMenu : public QMenu
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    WorkspaceMenu(QWidget *parent = nullptr);

private:

    /**
     * Get custom icon for the menu
     * @return Custom menu icon
     */
    QIcon getIcon();

private:
    hdps::gui::TriggerAction    _openWorkspaceAction;       /** Action for opening a workspace from file */
    hdps::gui::TriggerAction    _saveWorkspaceAction;       /** Action for saving the current workspace to file */
    hdps::gui::TriggerAction    _saveWorkspaceAsAction;     /** Action for saving the current workspace to a different file */
};
