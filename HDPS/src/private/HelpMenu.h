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
};
