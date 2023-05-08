#include "SharedParametersPlugin.h"

#include <Application.h>

Q_PLUGIN_METADATA(IID "NL.ManiVault.SharedParametersPlugin")

using namespace hdps;

SharedParametersPlugin::SharedParametersPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _publicActionsModel(this),
    _actionsWidget(&getWidget(), _publicActionsModel, "Shared Parameter"),
    _expertModeAction(this, "Expert mode", false, false)
{
    _expertModeAction.setIcon(Application::getIconFont("FontAwesome").getIcon("ellipsis-h"));
    _expertModeAction.setToolTip("In expert mode, all descendants of a root public parameter are displayed, otherwise they are hidden");

    //connect(&_expertModeAction, &ToggleAction::toggled, this, &ActionsFilterModel::invalidate);

    _actionsWidget.setRequestContextMenuCallback([this](QMenu* menu, WidgetActions selectedActions) -> void {
        if (selectedActions.count() == 1 && selectedActions.first()->isPublic()) {
            const auto childPublicActions = selectedActions.first()->getChildPublicActions();

            if (!childPublicActions.isEmpty()) {
                menu->addSeparator();

                auto showAllAction = new TriggerAction(menu, "Show all...");

                showAllAction->setIcon(Application::getIconFont("FontAwesome").getIcon("list"));

                connect(showAllAction, &TriggerAction::triggered, this, [this, selectedActions]() -> void {
                    for (auto childPublicAction : selectedActions.first()->getChildPublicActions())
                        _publicActionsModel.addPublicAction(childPublicAction, false);
                });

                menu->addAction(showAllAction);
            }
        }
    });

    auto& treeView = _actionsWidget.getHierarchyWidget().getTreeView();

    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::Location), false);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::Visible), true);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::MayPublish), true);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::MayConnect), true);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::MayDisconnect), true);
    treeView.setColumnHidden(static_cast<int>(AbstractActionsModel::Column::SortIndex), true);

    _actionsWidget.getHierarchyWidget().getToolbarLayout().addWidget(_expertModeAction.createWidget(&getWidget(), ToggleAction::PushButtonIcon));
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
