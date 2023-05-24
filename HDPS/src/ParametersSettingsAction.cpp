#include "ParametersSettingsAction.h"

namespace hdps
{

ParametersSettingsAction::ParametersSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "Parameters"),
    _askForSharedParameterNameAction(this, "Ask for name when publishing a parameter"),
    _confirmRemoveSharedParameterAction(this, "Confirm remove shared parameter")
{
    setShowLabels(false);

    _askForSharedParameterNameAction.setSettingsPrefix(getSettingsPrefix() + "AskForSharedParameterName");
    _confirmRemoveSharedParameterAction.setSettingsPrefix(getSettingsPrefix() + "ConfirmRemoveSharedParameter");
}

}
