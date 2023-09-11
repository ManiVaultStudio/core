// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataPropertiesPlugin.h"

#include <Application.h>
#include <CoreInterface.h>
#include <AbstractDataHierarchyManager.h>
#include <widgets/ViewPluginEditorDialog.h>
#include <Set.h>

Q_PLUGIN_METADATA(IID "nl.BioVault.DataPropertiesPlugin")

using namespace hdps;

DataPropertiesPlugin::DataPropertiesPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _dataPropertiesWidget(nullptr),
    _additionalEditorAction(this, "Edit dataset actions..."),
    _dataset()
{
    getWidget().addAction(&_additionalEditorAction);

    _additionalEditorAction.setIcon(Application::getIconFont("FontAwesome").getIcon("cogs"));
    _additionalEditorAction.setShortcut(tr("F11"));
    _additionalEditorAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _additionalEditorAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _additionalEditorAction.setConnectionPermissionsToForceNone();

    connect(&Application::core()->getDataHierarchyManager(), &AbstractDataHierarchyManager::selectedItemsChanged, this, &DataPropertiesPlugin::selectedItemsChanged);

    connect(&_additionalEditorAction, &TriggerAction::triggered, this, [this]() -> void {
        if (!_dataset.isValid())
            return;

        auto* viewPluginEditorDialog = new hdps::gui::ViewPluginEditorDialog(nullptr, dynamic_cast<WidgetAction*>(_dataset.get()->getActions().first()->parent()));
        connect(viewPluginEditorDialog, &hdps::gui::ViewPluginEditorDialog::finished, viewPluginEditorDialog, &hdps::gui::ViewPluginEditorDialog::deleteLater);
        viewPluginEditorDialog->open();
        });

}

void DataPropertiesPlugin::selectedItemsChanged(DataHierarchyItems selectedItems)
{
    if (projects().isOpeningProject() || projects().isImportingProject())
        return;

    if (selectedItems.isEmpty()) {
        return;
    }

    // Save reference to currently selected dataset
    _dataset = selectedItems.first()->getDataset();
}

void DataPropertiesPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(6, 6, 6, 6);

    layout->addWidget(&_dataPropertiesWidget);

    getWidget().setLayout(layout);

    connect(&dataHierarchy(), &AbstractDataHierarchyManager::selectedItemsChanged, this, [this](DataHierarchyItems selectedItems) -> void {
        if (selectedItems.isEmpty())
            getWidget().setWindowTitle("Data properties");
        else
            getWidget().setWindowTitle("Data properties: " + selectedItems.first()->getLocation());
    });

    getWidget().setWindowTitle("Data properties");
}

DataPropertiesPluginFactory::DataPropertiesPluginFactory() :
    ViewPluginFactory(true)
{
}

QIcon DataPropertiesPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("sliders-h", color);
}

ViewPlugin* DataPropertiesPluginFactory::produce()
{
    return new DataPropertiesPlugin(this);
}
