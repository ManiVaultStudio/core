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
     */
    ViewMenu(QWidget *parent = nullptr);

private:
    QMenu   _standardViewsMenu;     /** Menu for loading standard views */
    QMenu   _loadedViewsMenu;       /** Menu for toggling loaded views */
};
