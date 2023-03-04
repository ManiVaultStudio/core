#pragma once

#include <actions/TriggerAction.h>

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

private:
    hdps::gui::TriggerAction    _projectAboutAction;    /** Action for triggering the project about splash screen */
};
