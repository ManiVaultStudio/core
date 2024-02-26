// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginsStatusBarAction.h"

#ifdef _DEBUG
    #define PLUGINS_STATUS_BAR_ACTION_VERBOSE
#endif

#include <QHeaderView>

using namespace mv;
using namespace mv::gui;

PluginsStatusBarAction::PluginsStatusBarAction(QObject* parent, const QString& title) :
    StatusBarAction(parent, title),
    _barGroupAction(this, "Bar group"),
    _iconAction(this, "Icon"),
    _loadedPluginsAction(this, "Loaded Plugins"),
    _loadPluginBrowserAction(this, "Plugin"),
    _popupGroupAction(this, "Popup Group"),
    _model(),
    _filterModel(),
    _pluginsAction(this, "Plugins")
{
    setBarAction(&_barGroupAction);
    setPopupAction(&_pluginsAction);

    _barGroupAction.setShowLabels(false);

    _barGroupAction.addAction(&_iconAction);
    _barGroupAction.addAction(&_loadedPluginsAction, -1, [](WidgetAction* action, QWidget* widget) -> void {
        auto labelWidget = widget->findChild<QLabel*>("Label");
    });

    _iconAction.setEnabled(false);
    _iconAction.setDefaultWidgetFlags(StringAction::Label);
    _iconAction.setString(Application::getIconFont("FontAwesome").getIconCharacter("plug"));
    _iconAction.setWidgetConfigurationFunction([](WidgetAction* action, QWidget* widget) -> void {
        auto labelWidget = widget->findChild<QLabel*>("Label");

        Q_ASSERT(labelWidget != nullptr);

        if (labelWidget == nullptr)
            return;

        labelWidget->setFont(Application::getIconFont("FontAwesome").getFont());
    });

    _loadedPluginsAction.setEnabled(false);
    _loadedPluginsAction.setDefaultWidgetFlags(StringAction::Label);
    _loadedPluginsAction.setString(QString("%1 plugins").arg(QString::number(mv::plugins().getPluginFactoriesByTypes().size())));

    _popupGroupAction.setShowLabels(false);
    _popupGroupAction.setDefaultWidgetFlag(GroupAction::NoMargins);
    _pluginsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoGroupBoxInPopupLayout);
    _pluginsAction.setPopupSizeHint(QSize(500, 400));

    _popupGroupAction.addAction(&_pluginsAction);

    _pluginsAction.initialize(&_model, &_filterModel, "Plugin");
    _pluginsAction.setWidgetConfigurationFunction([this](WidgetAction* action, QWidget* widget) -> void {
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

        //treeView->setRootIsDecorated(false);

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
}
