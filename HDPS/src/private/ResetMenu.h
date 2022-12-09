#pragma once

#include <actions/TriggerAction.h>

#include <QMenu>

/**
 * Reset menu class
 * 
 * Menu class for resetting a project (or parts of it)
 * 
 * @author Thomas Kroes
 */
class ResetMenu : public QMenu
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    ResetMenu(QWidget* parent = nullptr);

private:
    hdps::gui::TriggerAction    _resetDataAction;       /** Action for resetting the data hierarchy */
    hdps::gui::TriggerAction    _resetLayoutAction;     /** Action for resetting the view plugins layout */
    hdps::gui::TriggerAction    _resetAllAction;        /** Action for resetting both the data hierarchy and the view plugins layout */
};
