#pragma once

#include "GlobalSettingsGroupAction.h"

#include "actions/ToggleAction.h"

namespace hdps
{

/**
 * Miscellaneous global settings action class
 *
 * Action class which groups all miscellaneous global settings
 *
 * @author Thomas Kroes
 */
class MiscellaneousSettingsAction final : public GlobalSettingsGroupAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    MiscellaneousSettingsAction(QObject* parent);

public: // Action getters

    gui::ToggleAction& getIgnoreLoadingErrorsAction() { return _ignoreLoadingErrorsAction; }

private:
    gui::ToggleAction   _ignoreLoadingErrorsAction;     /** Toggle action for ignoring loading errors */
};

}
