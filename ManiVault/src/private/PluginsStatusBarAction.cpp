// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginsStatusBarAction.h"

#include <Application.h>
#include <CoreInterface.h>

#include <widgets/HierarchyWidget.h>

#ifdef _DEBUG
    #define PLUGINS_STATUS_BAR_ACTION_VERBOSE
#endif

#include <QHeaderView>

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

PluginsStatusBarAction::PluginsStatusBarAction(QObject* parent, const QString& title) :
    StatusBarAction(parent, title, "plug"),
    _loadPluginBrowserAction(this, "Plugin"),
    _treeModel(),
    _listModel(),
    _filterModel(),
    _pluginsAction(this, "Plugins")
{
    setPopupAction(&_pluginsAction);
    setToolTip("Explore loaded plugins");

    _pluginsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoGroupBoxInPopupLayout);
    _pluginsAction.setPopupSizeHint(QSize(300, 200));

    _pluginsAction.initialize(&_treeModel, &_filterModel, "Plugin");
    _pluginsAction.setWidgetConfigurationFunction([this](WidgetAction* action, QWidget* widget) -> void {
        auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget != nullptr);

        if (hierarchyWidget == nullptr)
            return;

        hierarchyWidget->setHeaderHidden(true);
        hierarchyWidget->setWindowIcon(StyledIcon("plug"));
        
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
}
