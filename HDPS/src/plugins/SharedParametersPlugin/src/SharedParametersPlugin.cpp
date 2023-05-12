#include "SharedParametersPlugin.h"

#include <Application.h>

Q_PLUGIN_METADATA(IID "NL.ManiVault.SharedParametersPlugin")

using namespace hdps;

SharedParametersPlugin::SharedParametersPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _publicActionsModel(this),
    _actionsWidget(&getWidget(), _publicActionsModel, "Shared Parameter"),
    _expertModeAction(this, "Expert mode")
{
    _expertModeAction.setChecked(!_actionsWidget.getFilterModel().getPublicRootOnlyAction().isChecked());
    _expertModeAction.setIcon(Application::getIconFont("FontAwesome").getIcon("user-graduate"));
    _expertModeAction.setDefaultWidgetFlags(ToggleAction::PushButtonIcon);

    const auto updateExpertModeActionTooltip = [this]() -> void {
        if (_expertModeAction.isChecked())
            _expertModeAction.setToolTip("Display all parameter connections (expert mode)");
        else
            _expertModeAction.setToolTip("Display simplified parameter connections");
    };

    updateExpertModeActionTooltip();

    connect(&_expertModeAction, &ToggleAction::toggled, this, [this, updateExpertModeActionTooltip](bool toggled) -> void {
        _actionsWidget.getFilterModel().getPublicRootOnlyAction().setChecked(!toggled);
        updateExpertModeActionTooltip();
    });

    auto& hierarchyWidget = _actionsWidget.getHierarchyWidget();

    auto& treeView = hierarchyWidget.getTreeView();

    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::ForceDisabled), true);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::ForceHidden), true);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::Location), false);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::MayPublish), true);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::MayConnect), true);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::MayDisconnect), true);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::SortIndex), true);

    hierarchyWidget.getFilterGroupAction().setPopupSizeHint(QSize(350, 0));
    hierarchyWidget.getToolbarAction().addAction(&_expertModeAction);
}

void SharedParametersPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(6, 6, 6, 6);

    layout->addWidget(&_actionsWidget);

    getWidget().setLayout(layout);
}

ActionsPluginFactory::ActionsPluginFactory() :
    ViewPluginFactory(true)
{
}

QIcon ActionsPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("cloud", color);
}

ViewPlugin* ActionsPluginFactory::produce()
{
    return new SharedParametersPlugin(this);
}
