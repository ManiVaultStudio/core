// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataPropertiesPlugin.h"

#include <Application.h>
#include <CoreInterface.h>
#include <Set.h>

#include <widgets/ViewPluginEditorDialog.h>

Q_PLUGIN_METADATA(IID "nl.BioVault.DataPropertiesPlugin")

using namespace mv;

DataPropertiesPlugin::DataPropertiesPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _additionalEditorAction(this, "Edit dataset parameters..."),
    _dataPropertiesWidget(this, nullptr)
{
    _additionalEditorAction.setIcon(Application::getIconFont("FontAwesome").getIcon("cogs"));
    _additionalEditorAction.setShortcut(tr("F11"));
    _additionalEditorAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _additionalEditorAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu);
    _additionalEditorAction.setConnectionPermissionsToForceNone();
    _additionalEditorAction.setEnabled(false);

    addTitleBarMenuAction(&_additionalEditorAction);

    connect(&_additionalEditorAction, &TriggerAction::triggered, this, [this]() -> void {
        const auto selectedDataHierarchyItems = mv::dataHierarchy().getSelectedItems();

        if (selectedDataHierarchyItems.count() != 1)
            return;

        auto* viewPluginEditorDialog = new mv::gui::ViewPluginEditorDialog(nullptr, dynamic_cast<WidgetAction*>(selectedDataHierarchyItems.first()->getDataset().get()));
        
        connect(viewPluginEditorDialog, &mv::gui::ViewPluginEditorDialog::finished, viewPluginEditorDialog, &mv::gui::ViewPluginEditorDialog::deleteLater);
        
        viewPluginEditorDialog->open();
    });
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
}

QIcon DataPropertiesPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("sliders-h", color);
}

ViewPlugin* DataPropertiesPluginFactory::produce()
{
    return new DataPropertiesPlugin(this);
}