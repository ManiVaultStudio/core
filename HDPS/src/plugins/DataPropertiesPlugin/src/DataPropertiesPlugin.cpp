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

using namespace mv;

DataPropertiesPlugin::DataPropertiesPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _dataPropertiesWidget(nullptr),
    _additionalEditorAction(this, "Edit dataset parameters..."),
    _dataset(nullptr)
{
    _additionalEditorAction.setIcon(Application::getIconFont("FontAwesome").getIcon("cogs"));
    _additionalEditorAction.setShortcut(tr("F11"));
    _additionalEditorAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _additionalEditorAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu);
    _additionalEditorAction.setConnectionPermissionsToForceNone();
    _additionalEditorAction.setEnabled(false);

    addTitleBarMenuAction(&_additionalEditorAction);

    connect(&Application::core()->getDataHierarchyManager(), &AbstractDataHierarchyManager::selectedItemsChanged, this, &DataPropertiesPlugin::selectedItemsChanged, Qt::DirectConnection);

    connect(&_additionalEditorAction, &TriggerAction::triggered, this, [this]() -> void {
        
        if (!_dataset.isValid())
            return;

        auto* viewPluginEditorDialog = new mv::gui::ViewPluginEditorDialog(nullptr, dynamic_cast<WidgetAction*>(_dataset.get()));
        connect(viewPluginEditorDialog, &mv::gui::ViewPluginEditorDialog::finished, viewPluginEditorDialog, &mv::gui::ViewPluginEditorDialog::deleteLater);
        viewPluginEditorDialog->open();
        });

}

void DataPropertiesPlugin::selectedItemsChanged(DataHierarchyItems selectedItems)
{
    if (projects().isOpeningProject() || projects().isImportingProject())
        return;

    QString windowTitle = "Data Properties";

    if (!selectedItems.isEmpty()) {
        if (selectedItems.count() == 1)
            windowTitle += " - " + selectedItems.first()->getDataset()->getGuiName();
        else
            windowTitle += " - ...";
    } else {}

    getGuiNameAction().setString(windowTitle);

    if (selectedItems.isEmpty()) {
        _dataset = nullptr;
        _additionalEditorAction.setEnabled(false);
        return;
    }

    _dataset = selectedItems.first()->getDataset();

    _additionalEditorAction.setEnabled(true);
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
