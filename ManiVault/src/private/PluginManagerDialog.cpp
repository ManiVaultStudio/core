// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginManagerDialog.h"

#include <Application.h>
#include <CoreInterface.h>
#include <Plugin.h>

#include <QVBoxLayout>
#include <QMenu>
#include <QHeaderView>

using namespace mv;
using namespace mv::gui;

#ifdef _DEBUG
    #define PLUGIN_MANAGER_DIALOG_VERBOSE
#endif

PluginManagerDialog::PluginManagerDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _filterModel(this),
    _hierarchyWidget(this, "Plugin", _model, &_filterModel),
    _okAction(this, "Ok")
{
    const auto pluginIcon = StyledIcon("plug");

    setWindowIcon(pluginIcon);
    setWindowModality(Qt::ApplicationModal);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowTitle("Plugin Browser");
    
    auto layout = new QVBoxLayout();

    layout->addWidget(&_hierarchyWidget, 1);

    auto bottomLayout = new QHBoxLayout();

    bottomLayout->addWidget(_filterModel.getInstantiatedPluginsOnlyAction().createWidget(this));
    bottomLayout->addStretch(1);
    bottomLayout->addWidget(_okAction.createWidget(this));

    layout->addLayout(bottomLayout);

    setLayout(layout);

    _okAction.setToolTip("Exit the plugin manager");

    _hierarchyWidget.setWindowIcon(pluginIcon);
    _hierarchyWidget.getTreeView().setRootIsDecorated(true);

    _hierarchyWidget.getFilterGroupAction().addAction(&_filterModel.getInstantiatedPluginsOnlyAction());

    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setColumnHidden(static_cast<int>(AbstractPluginsModel::Column::Category), true);
    treeView.setColumnHidden(static_cast<int>(AbstractPluginsModel::Column::Id), true);

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);

    treeViewHeader->resizeSection(static_cast<int>(AbstractPluginsModel::Column::Name), 300);
    treeViewHeader->resizeSection(static_cast<int>(AbstractPluginsModel::Column::Category), 70);
    treeViewHeader->resizeSection(static_cast<int>(AbstractPluginsModel::Column::Id), 240);

    treeViewHeader->setSectionResizeMode(0, QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(1, QHeaderView::Fixed);

    connect(&treeView, &QTreeView::customContextMenuRequested, [this](const QPoint& point) {
        const auto selectedRows = _hierarchyWidget.getTreeView().selectionModel()->selectedRows();

        if (selectedRows.isEmpty())
            return;

        auto mayDestroyPlugins = true;

        for (const auto& selectedRow : selectedRows) {
            if (selectedRow.siblingAtColumn(1).data().toString() != "Instance") {
                mayDestroyPlugins = false;
                break;
            }
        }

        if (!mayDestroyPlugins)
            return;

        QMenu contextMenu;

        contextMenu.addAction(StyledIcon("trash"), QString("Destroy %1 plugin%2").arg(QString::number(selectedRows.count()), selectedRows.count() >= 2 ? "s" : ""), [this, selectedRows] {
            QList<plugin::Plugin*> plugins;

            for (const auto& selectedRow : selectedRows)
                plugins << selectedRow.data(Qt::UserRole + 1).value<plugin::Plugin*>();

            for (auto plugin : plugins)
                Application::core()->getPluginManager().destroyPlugin(plugin);
        });

        contextMenu.exec(QCursor::pos());
    });

    connect(&plugins(), &AbstractPluginManager::pluginAboutToBeDestroyed, this, [this](plugin::Plugin* plugin) -> void {
        const auto matches = _model.findItems(plugin->getId(), Qt::MatchFlag::MatchExactly | Qt::MatchFlag::MatchRecursive, static_cast<int>(AbstractPluginsModel::Column::Id));

        if (matches.isEmpty())
            return;

        const auto firstIndex = matches.first()->index();

        _model.removeRow(firstIndex.row(), firstIndex.parent());
    });

    connect(&_okAction, &TriggerAction::triggered, this, &PluginManagerDialog::accept);
}

void PluginManagerDialog::create()
{
    PluginManagerDialog loadedPluginsDialog;

    loadedPluginsDialog.exec();
}
