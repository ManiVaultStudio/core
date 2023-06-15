#pragma once

#include <QMenu>

#include <actions/ToggleAction.h>

/**
 * Mode menu class
 * 
 * Menu class for toggling between different modes
 * 
 * @author Thomas Kroes
 */
class ModeMenu : public QMenu
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    ModeMenu(QWidget *parent = nullptr);

private:
    
};
