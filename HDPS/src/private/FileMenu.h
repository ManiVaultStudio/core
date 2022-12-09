#pragma once

#include "RecentProjectsMenu.h"
#include "ImportDataMenu.h"
#include "ResetMenu.h"

#include <actions/TriggerAction.h>

#include <QMenu>

/**
 * File menu class
 * 
 * Menu class for file-related operations
 * 
 * @author Thomas Kroes
 */
class FileMenu : public QMenu
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    FileMenu(QWidget *parent = nullptr);

private:
    hdps::gui::TriggerAction    _openProjectAction;         /** Action for opening a project */
    hdps::gui::TriggerAction    _saveProjectAction;         /** Action for saving a project */
    hdps::gui::TriggerAction    _saveProjectAsAction;       /** Action for saving a project under a new name */
    RecentProjectsMenu          _recentProjectsMenu;        /** Menu for loading recent projects */
    ResetMenu                   _resetMenu;                 /** Menu for resetting the project (or parts of it) */
    ImportDataMenu              _importDataMenu;            /** Menu for importing data */
    hdps::gui::TriggerAction    _pluginManagerAction;   /** Action for showing the loaded plugins dialog */
    hdps::gui::TriggerAction    _publishAction;             /** Action for publishing the project to an end-user */
    hdps::gui::TriggerAction    _exitAction;                /** Action for exiting the application */
};
