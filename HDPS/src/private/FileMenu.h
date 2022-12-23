#pragma once

#include <actions/TriggerAction.h>

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

    /**
     * Invoked when the menu is show on the screen
     * @param showEvent Pointer to show event
     */
    void showEvent(QShowEvent* showEvent) override;

private:
    hdps::gui::TriggerAction    _exitApplictionAction;  /** Trigger action to exit the application */
};
