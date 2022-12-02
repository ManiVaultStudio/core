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

    /**
     * Invoked when the menu is shown
     * @param showEvent Pointer to the show event
     */
    void showEvent(QShowEvent* showEvent) override;
};
