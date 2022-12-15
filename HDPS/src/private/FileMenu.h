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

    void showEvent(QShowEvent* showEvent) override;

private:
    hdps::gui::TriggerAction    _publishAction;             /** Action for publishing the project to an end-user */
    hdps::gui::TriggerAction    _pluginManagerAction;       /** Action for showing the loaded plugins dialog */
    hdps::gui::TriggerAction    _globalSettingsAction;      /** Action for modifying global settings */
    hdps::gui::TriggerAction    _exitAction;                /** Action for exiting the application */
};
