// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SharedParametersPlugin.h"

#include <Application.h>

Q_PLUGIN_METADATA(IID "studio.manivault.SharedParametersPlugin")

using namespace mv;

SharedParametersPlugin::SharedParametersPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _publicActionsModel(this),
    _actionsWidget(&getWidget(), _publicActionsModel, "Shared Parameter")
{
    _actionsWidget.getFilterModel().getPublicRootOnlyAction().setChecked(true);

    auto& exportMode = mv::settings().getParametersSettings().getExpertModeAction();

    const auto updateRootOnly = [this, &exportMode]() -> void {
        _actionsWidget.getFilterModel().getPublicRootOnlyAction().setChecked(!exportMode.isChecked());
    };

    updateRootOnly();

    connect(&mv::settings().getParametersSettings().getExpertModeAction(), &ToggleAction::toggled, this, updateRootOnly);

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

    hierarchyWidget.getToolbarAction().addAction(&mv::settings().getParametersSettings().getExpertModeAction(), ToggleAction::PushButtonIcon);

    getLearningCenterAction().addVideos(QStringList({ "Practitioner", "Developer" }));
    getLearningCenterAction().addTutorials(QStringList({ "GettingStarted", "SharedParametersPlugin" }));
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
    getPluginMetadata().setDescription("For interacting with shared parameters");
    getPluginMetadata().setSummary("This system view plugin is for interacting with shared parameters.");
    getPluginMetadata().setCopyrightHolder({ "BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)" });
    getPluginMetadata().setAuthors({
        { "T. Kroes", "Lead software architect", "" }
    });
    getPluginMetadata().setOrganizations({
        { "LUMC", "Leiden University Medical Center", "https://www.lumc.nl/en/" },
        { "TU Delft", "Delft university of technology", "https://www.tudelft.nl/" }
	});
    getPluginMetadata().setLicenseText("This plugin is distributed under the [LGPL v3.0](https://www.gnu.org/licenses/lgpl-3.0.en.html) license.");
}

QIcon SharedParametersPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("cloud", color);
}

QUrl SharedParametersPluginFactory::getReadmeMarkdownUrl() const
{
#ifdef ON_LEARNING_CENTER_FEATURE_BRANCH
    return QUrl("https://raw.githubusercontent.com/ManiVaultStudio/core/feature/learning_center/ManiVault/src/plugins/SharedParametersPlugin/README.md");
#else
    return QUrl("https://raw.githubusercontent.com/ManiVaultStudio/core/master/ManiVault/src/plugins/SharedParametersPlugin/README.md");
#endif
}

QUrl SharedParametersPluginFactory::getRepositoryUrl() const
{
    return QUrl("https://github.com/ManiVaultStudio/core");
}

ViewPlugin* SharedParametersPluginFactory::produce()
{
    return new SharedParametersPlugin(this);
}
