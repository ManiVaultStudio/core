// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginLearningCenterAction.h"

using namespace mv::util;

namespace mv::gui {

PluginLearningCenterAction::PluginLearningCenterAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _plugin(nullptr),
    _actions(this, "Actions"),
    _viewDescriptionAction(this, "View description"),
    _viewHelpAction(this, "View help"),
    _viewShortcutMapAction(this, "View shortcuts")
{
    //connect(&_viewDescriptionAction, &TriggerAction::triggered, _plugin, &Plugin::viewDescription);

    _viewDescriptionAction.setToolTip("View description");
    _viewDescriptionAction.setIconByName("book-reader");
    _viewDescriptionAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu);
    _viewDescriptionAction.setConnectionPermissionsToForceNone();

    //_viewHelpAction.setToolTip(QString("Shows %1 documentation").arg(factory->getKind()));
    //_viewHelpAction.setShortcut(tr("F1"));
    //_viewHelpAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    //_viewHelpAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu, false);
    //_viewHelpAction.setConnectionPermissionsToForceNone();

    connect(&_viewHelpAction, &TriggerAction::triggered, this, [this]() -> void {
        Q_ASSERT(_plugin);

        if (!_plugin)
            return;

        const_cast<plugin::PluginFactory*>(_plugin->getFactory())->getTriggerHelpAction().trigger();
    });
}

void PluginLearningCenterAction::initialize(plugin::Plugin* plugin)
{
    Q_ASSERT(plugin);

    if (!plugin)
        return;

    _plugin = plugin;
}

bool PluginLearningCenterAction::hasHelp() const
{
    return _plugin->getFactory()->hasHelp();
}

void PluginLearningCenterAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);
}

QVariantMap PluginLearningCenterAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    return variantMap;
}

void PluginLearningCenterAction::addVideo(const util::Video& video)
{
    _videos.push_back(video);
}

void PluginLearningCenterAction::addVideos(const util::Videos& videos)
{
    _videos.insert(_videos.end(), videos.begin(), videos.end());
}

void PluginLearningCenterAction::addVideos(const QString& tag)
{
    addVideos(mv::help().getVideos({ tag }));
}

void PluginLearningCenterAction::addVideos(const QStringList& tags)
{
    addVideos(mv::help().getVideos(tags));
}

const util::Videos& PluginLearningCenterAction::getVideos() const
{
    return _videos;
}

}

//void ViewPlugin::view()
//{
//#ifdef VIEW_PLUGIN_VERBOSE
//    qDebug() << __FUNCTION__;
//#endif
//
//    if (!_shortcutMapOverlayWidget.isNull())
//        return;
//
//    _shortcutMapOverlayWidget = getMap().createShortcutMapOverlayWidget(&_widget);
//
//    _shortcutMapOverlayWidget->show();
//}
//
//void ViewPlugin::viewDescription()
//{
//#ifdef VIEW_PLUGIN_VERBOSE
//    qDebug() << __FUNCTION__;
//#endif
//}

//bool Plugin::hasHelp()
//{
//    return const_cast<PluginFactory*>(_factory)->hasHelp();
//}
//
//mv::gui::TriggerAction& Plugin::getTriggerHelpAction()
//{
//    return const_cast<PluginFactory*>(_factory)->getTriggerHelpAction();
//}