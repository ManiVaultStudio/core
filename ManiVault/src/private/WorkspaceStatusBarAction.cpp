// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkspaceStatusBarAction.h"

#include <actions/LockingAction.h>

#ifdef _DEBUG
    #define WORKSPACE_STATUS_BAR_ACTION_VERBOSE
#endif

using namespace mv;
using namespace mv::gui;

WorkspaceStatusBarAction::WorkspaceStatusBarAction(QObject* parent, const QString& title) :
    StatusBarAction(parent, title, "table"),
    _loadPluginBrowserAction(this, "Plugin"),
    _settingsAction(this, "Settings")
{
    setPopupAction(&_settingsAction);
    setToolTip("Explore workspace settings");

    connect(&mv::workspaces(), &AbstractWorkspaceManager::workspaceCreated, this, [this](const mv::Workspace& workspace) -> void {
        _settingsAction.removeAllActions();
        _settingsAction.addAction(const_cast<LockingAction*>(&workspace.getLockingAction()));
    });

    //_pluginsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoGroupBoxInPopupLayout);
    _settingsAction.setPopupSizeHint(QSize(300, 200));

    /*
    _settingsAction.setWidgetConfigurationFunction([this](WidgetAction* action, QWidget* widget) -> void {
        auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget != nullptr);

        if (hierarchyWidget == nullptr)
            return;

        hierarchyWidget->setHeaderHidden(true);
        hierarchyWidget->setWindowIcon(Application::getIconFont("FontAwesome").getIcon("plug"));
        
        hierarchyWidget->getFilterGroupAction().setVisible(false);
        hierarchyWidget->getColumnsGroupAction().setVisible(false);

        auto& toolbarAction = hierarchyWidget->getToolbarAction();

        toolbarAction.addAction(&_loadPluginBrowserAction);

        auto treeView = widget->findChild<QTreeView*>("TreeView");

        Q_ASSERT(treeView != nullptr);

        if (treeView == nullptr)
            return;

        treeView->setColumnHidden(static_cast<int>(AbstractPluginFactoriesModel::Column::NumberOfInstances), true);

        auto treeViewHeader = treeView->header();

        treeViewHeader->setStretchLastSection(false);

        treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractPluginFactoriesModel::Column::Name), QHeaderView::Stretch);
        treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractPluginFactoriesModel::Column::Version), QHeaderView::ResizeToContents);

        treeViewHeader->resizeSections(QHeaderView::ResizeToContents);
    });
    

    _loadPluginBrowserAction.setIconByName("window-maximize");
    _loadPluginBrowserAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);
    _loadPluginBrowserAction.setToolTip("Load plugin browser");

    connect(&_loadPluginBrowserAction, &TriggerAction::triggered, this, [this]() -> void {
        projects().getPluginManagerAction().trigger();
    });

    auto& badge = getBarIconStringAction().getBadge();

    badge.setScale(0.55f);
    badge.setBackgroundColor(qApp->palette().highlight().color());

    const auto updateBadge = [this, &badge]() -> void {
        const auto numberPluginsLoaded = _listModel.rowCount();

        badge.setEnabled(numberPluginsLoaded > 0);
        badge.setNumber(numberPluginsLoaded);
    };

    updateBadge();

    connect(&_listModel, &QSortFilterProxyModel::rowsInserted, this, updateBadge);
    connect(&_listModel, &QSortFilterProxyModel::rowsRemoved, this, updateBadge);
    */
}
