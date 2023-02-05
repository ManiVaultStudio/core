#pragma once

#include "AbstractSettingsManager.h"

#include <actions/GroupAction.h>

namespace hdps
{

class SettingsManager final : public AbstractSettingsManager
{
    Q_OBJECT

public:

    /** Default constructor */
    SettingsManager();

    /** Perform manager startup initialization */
    void initialize() override;

    /** Resets the contents of the actions manager */
    void reset() override;

    /** Opens the settings editor dialog */
    void edit() override;

public: // Action getters

    gui::TriggerAction& getEditSettingsAction() override { return _editSettingsAction; }
    gui::DirectoryPickerAction& getGlobalProjectsPathAction() override { return _globalProjectsPathAction; }
    gui::DirectoryPickerAction& getGlobalWorkspacesPathAction() override { return _globalWorkspacesPathAction; }
    gui::DirectoryPickerAction& getGlobalDataPathAction() override { return _globalDataPathAction; }
    gui::ToggleAction& getIgnoreLoadingErrorsAction() override { return _ignoreLoadingErrorsAction; }

private:
    gui::TriggerAction          _editSettingsAction;            /** Action for triggering the settings dialog */
    gui::DirectoryPickerAction  _globalProjectsPathAction;      /** Directory picker action for global projects directory  */
    gui::DirectoryPickerAction  _globalWorkspacesPathAction;    /** Directory picker action for global workspaces directory  */
    gui::DirectoryPickerAction  _globalDataPathAction;          /** Directory picker action for global data directory  */
    gui::ToggleAction           _ignoreLoadingErrorsAction;     /** Toggle action for ignoring loading errors */
};

}
