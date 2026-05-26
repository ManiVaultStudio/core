// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataPropertiesPlugin.h"

#include <ManiVaultVersion.h>

#include <Application.h>
#include <CoreInterface.h>
#include <Set.h>

#include <widgets/ViewPluginEditorDialog.h>

Q_PLUGIN_METADATA(IID "studio.manivault.DataPropertiesPlugin")

using namespace mv;

DataPropertiesPlugin::DataPropertiesPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _additionalEditorAction(this, "Edit dataset parameters..."),
    _dataPropertiesWidget(this, nullptr)
{
    _additionalEditorAction.setIcon(StyledIcon("gears"));
    _additionalEditorAction.setShortcut(tr("F11"));
    _additionalEditorAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _additionalEditorAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu);
    _additionalEditorAction.setConnectionPermissionsToForceNone();
    //_additionalEditorAction.setEnabled(false);

    addTitleBarMenuAction(&_additionalEditorAction);

    connect(&_additionalEditorAction, &TriggerAction::triggered, this, [this]() -> void {
        const auto selectedDataHierarchyItems = mv::dataHierarchy().getSelectedItems();

        if (selectedDataHierarchyItems.count() != 1)
            return;

        auto* viewPluginEditorDialog = new mv::gui::ViewPluginEditorDialog(nullptr, dynamic_cast<WidgetAction*>(selectedDataHierarchyItems.first()->getDataset().get()));
        
        connect(viewPluginEditorDialog, &mv::gui::ViewPluginEditorDialog::finished, viewPluginEditorDialog, &mv::gui::ViewPluginEditorDialog::deleteLater);
        
        viewPluginEditorDialog->open();
    });

    getLearningCenterAction().addVideos(QStringList({ "Practitioner", "Developer" }));
    getLearningCenterAction().addTutorials(QStringList({ "GettingStarted", "DataPropertiesPlugin" }));
}

void DataPropertiesPlugin::updateWindowTitle(DataHierarchyItems selectedDataHierarchyItems)
{
    QString windowTitle = "Data Properties";

    if (!selectedDataHierarchyItems.isEmpty()) {
        if (selectedDataHierarchyItems.count() == 1)
            windowTitle += " - " + selectedDataHierarchyItems.first()->getDataset()->getGuiName();
        else
            windowTitle += " - ...";
    }

    getGuiNameAction().setString(windowTitle);
}

void DataPropertiesPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(6, 6, 6, 6);

    layout->addWidget(&_dataPropertiesWidget);

    getWidget().setLayout(layout);
}

DataPropertiesPluginFactory::DataPropertiesPluginFactory() :
    ViewPluginFactory(true)
{
    setIconByName("sliders");

    getPluginMetadata().setDescription("For interacting with dataset properties");
    getPluginMetadata().setSummary("This view plugin is for interacting with dataset properties.");
    getPluginMetadata().setCopyrightHolder({ "BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)" });
    getPluginMetadata().setAuthors({
        { "T. Kroes", { "Lead software architect" }, { "LUMC" } },
        { "A. Vieth", { "Plugin developer", "Maintainer" }, { "LUMC", "TU Delft" } }
    });
    getPluginMetadata().setOrganizations({
        { "LUMC", "Leiden University Medical Center", "https://www.lumc.nl/en/" },
        { "TU Delft", "Delft university of technology", "https://www.tudelft.nl/" }
    });
    getPluginMetadata().setLicenseText("This plugin is distributed under the [LGPL v3.0](https://www.gnu.org/licenses/lgpl-3.0.en.html) license.");
}

QUrl DataPropertiesPluginFactory::getReadmeMarkdownUrl() const
{
    if constexpr (MV_IS_DEV())
        return { "https://raw.githubusercontent.com/ManiVaultStudio/core/tree/master/ManiVault/src/plugins/DataPropertiesPlugin" };
    else
        return QUrl{
            QString("https://raw.githubusercontent.com/ManiVaultStudio/core/tree/v") +
            QString::fromStdString(MV_VERSION_STRING()) +
            "/ManiVault/src/plugins/DataPropertiesPlugin/README.md"
    };
}

QUrl DataPropertiesPluginFactory::getRepositoryUrl() const
{
    if constexpr (MV_IS_DEV())
        return { "https://www.github.com/ManiVaultStudio/core/tree/master/ManiVault/src/plugins/DataPropertiesPlugin" };
    else
        return QUrl{
            QString("https://www.github.com/ManiVaultStudio/core/tree/v") +
            QString::fromStdString(MV_VERSION_STRING()) +
            "/ManiVault/src/plugins/DataPropertiesPlugin"
    };
}

ViewPlugin* DataPropertiesPluginFactory::produce()
{
    return new DataPropertiesPlugin(this);
}
