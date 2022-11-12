#pragma once

#include "RecentProjectsMenu.h"

#include <QMenu>

/**
 * Import data menu class
 * 
 * Menu class for importing data
 * 
 * @author Thomas Kroes
 */
class ImportDataMenu : public QMenu
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    ImportDataMenu(QWidget *parent = nullptr);

private:
    QAction                 _openProjectAction;         /** Action for opening a project */
    QAction                 _saveProjectAction;         /** Action for saving a project */
    QAction                 _saveProjectAsAction;       /** Action for saving a project under a new name */
    RecentProjectsMenu      _recentProjectsMenu;        /** Menu for loading recent projects */
    QAction                 _clearDatasetsAction;       /** Action for clearing all datasets */
    QAction                 _importDataMenu;            /** Action for import data */
    QAction                 _exitAction;                /** Action for exiting the application */
};
