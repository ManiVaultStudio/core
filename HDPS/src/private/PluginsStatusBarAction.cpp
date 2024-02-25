// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginsStatusBarAction.h"

#ifdef _DEBUG
    #define PLUGINS_STATUS_BAR_ACTION_VERBOSE
#endif

using namespace mv;
using namespace mv::gui;

PluginsStatusBarAction::PluginsStatusBarAction(QObject* parent, const QString& title) :
    StatusBarAction(parent, title),
    _barGroupAction(this, "Bar group"),
    _iconAction(this, "Icon"),
    _loadedPluginsAction(this, "Loaded Plugins"),
    _loadPluginAction(this, "Plugin"),
    _popupGroupAction(this, "Popup Group"),
    _pluginsAction(this, "Plugins")
{
    setBarAction(&_barGroupAction);
    setPopupAction(&_popupGroupAction);

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
    _loadedPluginsAction.setString(QString("%1 plugins loaded").arg(QString::number(mv::plugins().getPluginFactoriesByTypes().size())));

    _popupGroupAction.setShowLabels(false);
    _popupGroupAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoGroupBoxInPopupLayout);
    _popupGroupAction.addAction(&_pluginsAction);

}
