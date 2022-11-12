#pragma once

#include "RecentProjectsMenu.h"
#include "ImportDataMenu.h"

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
    QAction                 _openProjectAction;         /** Action for opening a project */
    QAction                 _saveProjectAction;         /** Action for saving a project */
    QAction                 _saveProjectAsAction;       /** Action for saving a project under a new name */
    RecentProjectsMenu      _recentProjectsMenu;        /** Menu for loading recent projects */
    QAction                 _clearDatasetsAction;       /** Action for clearing all datasets */
    ImportDataMenu          _importDataMenu;            /** Menu for importing data */
    QAction                 _exitAction;                /** Action for exiting the application */
};
