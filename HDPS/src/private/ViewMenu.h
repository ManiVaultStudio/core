#pragma once

#include <QMenu>

/**
 * View menu class
 * 
 * Menu class for managing viewing plugins
 * 
 * @author Thomas Kroes
 */
class ViewMenu : public QMenu
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param standardViews Whether to include the menu for creating standard views
     * @param loadedViews Whether to include the menu for toggling loaded views
     */
    ViewMenu(QWidget *parent = nullptr, bool standardViews = true, bool loadedViews = true);

private:
    QMenu   _standardViewsMenu;     /** Menu for loading standard views */
    QMenu   _loadedViewsMenu;       /** Menu for toggling loaded views */
};
