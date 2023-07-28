// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SharedParametersPlugin.h"

#include <Application.h>

Q_PLUGIN_METADATA(IID "NL.ManiVault.SharedParametersPlugin")

using namespace hdps;

SharedParametersPlugin::SharedParametersPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _publicActionsModel(this),
    _actionsWidget(&getWidget(), _publicActionsModel, "Shared Parameter")
{
    _actionsWidget.getFilterModel().getPublicRootOnlyAction().setChecked(true);

    auto& exportMode = hdps::settings().getParametersSettings().getExpertModeAction();

    const auto updateRootOnly = [this, &exportMode]() -> void {
        _actionsWidget.getFilterModel().getPublicRootOnlyAction().setChecked(!exportMode.isChecked());
    };

    updateRootOnly();

    connect(&hdps::settings().getParametersSettings().getExpertModeAction(), &ToggleAction::toggled, this, updateRootOnly);

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

    hierarchyWidget.getToolbarAction().addAction(&hdps::settings().getParametersSettings().getExpertModeAction(), ToggleAction::PushButtonIcon);
}

void SharedParametersPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(6, 6, 6, 6);

    layout->addWidget(&_actionsWidget);

    getWidget().setLayout(layout);
}

SharedParametersPluginFactory::SharedParametersPluginFactory() :
    ViewPluginFactory(true)
{
}

QIcon SharedParametersPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("cloud", color);
}

ViewPlugin* SharedParametersPluginFactory::produce()
{
    return new SharedParametersPlugin(this);
}
