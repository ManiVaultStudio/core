#pragma once

#include "AbstractSettingsManager.h"

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

public: // Global settings actions

    ParametersSettingsAction& getParameters() override { return _parametersSettingsAction; };
    MiscellaneousSettingsAction& getMiscellaneous() override { return _miscellaneousSettingsAction; };

private:
    gui::TriggerAction          _editSettingsAction;            /** Action for triggering the settings dialog */
    ParametersSettingsAction    _parametersSettingsAction;      /** Parameters global settings */
    MiscellaneousSettingsAction _miscellaneousSettingsAction;   /** Miscellaneous global settings */
};

}
