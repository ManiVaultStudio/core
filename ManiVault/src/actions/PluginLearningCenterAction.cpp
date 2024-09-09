// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginLearningCenterAction.h"

#include "widgets/ViewPluginLearningCenterOverlayWidget.h"
#include "widgets/ViewPluginDescriptionOverlayWidget.h"

using namespace mv::util;
using namespace mv::plugin;

namespace mv::gui {

PluginLearningCenterAction::PluginLearningCenterAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _plugin(nullptr),
    _actions(this, "Actions"),
    _viewDescriptionAction(this, "View description"),
    _viewHelpAction(this, "View help"),
    _viewShortcutsAction(this, "View shortcuts"),
    _viewPluginOverlayVisibleAction(this, "View plugin overlay visible", true),
    _learningCenterOverlayWidget(nullptr)
{
    _viewDescriptionAction.setToolTip(getShortDescription());
    _viewDescriptionAction.setIconByName("book-reader");
    _viewDescriptionAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu);
    _viewDescriptionAction.setConnectionPermissionsToForceNone();

    connect(&_viewDescriptionAction, &TriggerAction::triggered, this, &PluginLearningCenterAction::viewDescription);

    _viewHelpAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu, false);
    _viewHelpAction.setConnectionPermissionsToForceNone();

    connect(&_viewHelpAction, &TriggerAction::triggered, this, [this]() -> void {
        Q_ASSERT(_plugin);

        if (!_plugin)
            return;

        const_cast<plugin::PluginFactory*>(_plugin->getFactory())->getTriggerHelpAction().trigger();
    });

    connect(&_viewShortcutsAction, &TriggerAction::triggered, this, &PluginLearningCenterAction::viewShortcuts);

    _viewPluginOverlayVisibleAction.setToolTip("Toggle view plugin learning center toolbar visibility");
    _viewPluginOverlayVisibleAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu);
    _viewPluginOverlayVisibleAction.setConnectionPermissionsToForceNone();

    const auto updateViewPluginOverlayVisibleActionIcon = [this]() -> void {
        _viewPluginOverlayVisibleAction.setIconByName(_viewPluginOverlayVisibleAction.isChecked() ? "eye" : "eye-slash");
    };

    updateViewPluginOverlayVisibleActionIcon();

    connect(&_viewPluginOverlayVisibleAction, &ToggleAction::toggled, this, updateViewPluginOverlayVisibleActionIcon);
}

void PluginLearningCenterAction::initialize(plugin::Plugin* plugin)
{
    Q_ASSERT(plugin);

    if (!plugin)
        return;

    _plugin = plugin;

    _viewHelpAction.setToolTip(QString("Shows %1 documentation").arg(_plugin->getKind()));

    if (_shortDescription.isEmpty())
        setShortDescription(QString("View %1 description").arg(_plugin->getKind()));

    if (_longDescription.isEmpty())
        setShortDescription("No description available yet, stay tuned");

    if (_learningCenterOverlayWidget)
        _learningCenterOverlayWidget->deleteLater();
}

ViewPluginLearningCenterOverlayWidget* PluginLearningCenterAction::getViewPluginOverlayWidget() const
{
    return _learningCenterOverlayWidget;
}

void PluginLearningCenterAction::createViewPluginOverlayWidget()
{
    _learningCenterOverlayWidget = new ViewPluginLearningCenterOverlayWidget(&getViewPlugin()->getWidget(), getViewPlugin());
    _learningCenterOverlayWidget->show();
}

QString PluginLearningCenterAction::getPluginTitle() const
{
    return _pluginTitle.isEmpty() ? _plugin->getKind() : _pluginTitle;
}

void PluginLearningCenterAction::setPluginTitle(const QString& pluginTitle)
{
    if (pluginTitle == _pluginTitle)
        return;

    _pluginTitle = pluginTitle;

    emit pluginTitleChanged(_pluginTitle);
}

QString PluginLearningCenterAction::getShortDescription() const
{
    return _shortDescription;
}

void PluginLearningCenterAction::setShortDescription(const QString& shortDescription)
{
    if (shortDescription == _shortDescription)
        return;

    const auto previousShortDescription = _shortDescription;

    _shortDescription = shortDescription;

    emit shortDescriptionChanged(previousShortDescription, _shortDescription);
}

QString PluginLearningCenterAction::getLongDescription() const
{
    return _longDescription;
}

void PluginLearningCenterAction::setLongDescription(const QString& longDescription)
{
    if (longDescription == _longDescription)
        return;

    const auto previousLongDescription = _longDescription;

    _longDescription = longDescription;

    emit longDescriptionChanged(previousLongDescription, _longDescription);
}

bool PluginLearningCenterAction::hasDescription() const
{
    return !_longDescription.isEmpty();
}

bool PluginLearningCenterAction::hasHelp() const
{
    return _plugin->getFactory()->hasHelp();
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

const Videos& PluginLearningCenterAction::getVideos() const
{
    return _videos;
}

bool PluginLearningCenterAction::isViewPlugin() const
{
    Q_ASSERT(_plugin);

    if (!_plugin)
        return false;

    return _plugin->getType() == plugin::Type::VIEW;
}

plugin::ViewPlugin* PluginLearningCenterAction::getViewPlugin() const
{
    return dynamic_cast<ViewPlugin*>(_plugin);
}

void PluginLearningCenterAction::viewDescription()
{
#ifdef VIEW_PLUGIN_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (isViewPlugin() && hasDescription())
    {
        if (!_descriptionOverlayWidget.isNull())
            return;

        _descriptionOverlayWidget = new gui::ViewPluginDescriptionOverlayWidget(dynamic_cast<ViewPlugin*>(_plugin));

        _descriptionOverlayWidget->show();
    }
}

void PluginLearningCenterAction::viewShortcuts()
{
#ifdef VIEW_PLUGIN_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (isViewPlugin() && _plugin->getShortcuts().hasShortcuts())
    {
        if (!_shortcutsOverlayWidget.isNull())
            return;

        _shortcutsOverlayWidget = new gui::ViewPluginShortcutsOverlayWidget(dynamic_cast<ViewPlugin*>(_plugin));

        _shortcutsOverlayWidget->show();
    }
}

void PluginLearningCenterAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _viewPluginOverlayVisibleAction.fromParentVariantMap(variantMap);
}

QVariantMap PluginLearningCenterAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    _viewPluginOverlayVisibleAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
