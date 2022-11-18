#pragma once

#include <QMenu>

/**
 * Help menu class
 * 
 * Menu class for help functionality
 * 
 * @author Thomas Kroes
 */
class HelpMenu : public QMenu
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    HelpMenu(QWidget *parent = nullptr);

    /**
     * Invoked when the menu is shown
     * @param showEvent Pointer to the show event
     */
    void showEvent(QShowEvent* showEvent) override;

private:
    QMenu   _pluginsHelpMenu;   /** Menu for plugins help */
};
