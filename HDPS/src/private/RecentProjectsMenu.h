#pragma once

#include <QMenu>

/**
 * Recent projects menu class
 * 
 * Menu class for loading recent projects
 * 
 * @author Thomas Kroes
 */
class RecentProjectsMenu : public QMenu
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    RecentProjectsMenu(QWidget *parent = nullptr);

    /** Update the recent projects actions */
    void updateActions();
};
