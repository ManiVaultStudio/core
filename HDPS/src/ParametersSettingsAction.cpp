#include "ParametersSettingsAction.h"
#include "Application.h"

using namespace hdps::gui;

namespace hdps
{

ParametersSettingsAction::ParametersSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "Parameters"),
    _askForSharedParameterNameAction(this, "Ask for shared parameter name"),
    _confirmRemoveSharedParameterAction(this, "Confirm remove shared parameter"),
    _expertModeAction(this, "Expert mode", false)
{
    setShowLabels(false);

    _askForSharedParameterNameAction.setSettingsPrefix(getSettingsPrefix() + "AskForSharedParameterName");
    _confirmRemoveSharedParameterAction.setSettingsPrefix(getSettingsPrefix() + "ConfirmRemoveSharedParameter");
    _expertModeAction.setSettingsPrefix(getSettingsPrefix() + "ExpertMode");

    const auto updateExpertModeActionTooltip = [this]() -> void {
        if (_expertModeAction.isChecked())
            _expertModeAction.setToolTip("Display all parameter connections (expert mode)");
        else
            _expertModeAction.setToolTip("Display simplified parameter connections");
    };

    updateExpertModeActionTooltip();

    connect(&_expertModeAction, &ToggleAction::toggled, this, updateExpertModeActionTooltip);

    _expertModeAction.setChecked(false);
    _expertModeAction.setIcon(Application::getIconFont("FontAwesome").getIcon("user-graduate"));
    //_expertModeAction.setDefaultWidgetFlags(ToggleAction::PushButtonIcon);
}

}