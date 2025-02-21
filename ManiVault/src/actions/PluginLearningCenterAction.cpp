// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginLearningCenterAction.h"

#include "CoreInterface.h"
#include "Plugin.h"
#include "ViewPlugin.h"

#include "util/Miscellaneous.h"

#include "widgets/ViewPluginLearningCenterOverlayWidget.h"
#include "widgets/PluginShortcutsDialog.h"

using namespace mv::util;
using namespace mv::plugin;

namespace mv::gui {

const QStringList PluginLearningCenterAction::alignmentOptions = { "TopLeft", "TopRight", "BottomLeft", "BottomRight" };

const std::vector<Qt::Alignment> PluginLearningCenterAction::alignmentFlags = {
        Qt::AlignTop | Qt::AlignLeft,
        Qt::AlignTop | Qt::AlignRight,
        Qt::AlignBottom | Qt::AlignLeft,
        Qt::AlignBottom | Qt::AlignRight
};

PluginLearningCenterAction::PluginLearningCenterAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _plugin(nullptr),
    _actions(this, "Actions"),
    _viewHelpAction(this, "View help"),
    _toolbarVisibleAction(this, "Visible", true),
    _hideToolbarAction(this, "Hide toolbar"),
    _alignmentAction(this, "View plugin overlay alignment", alignmentOptions, "BottomLeft"),
    _moveToTopLeftAction(this, "Move to top-left"),
    _moveToTopRightAction(this, "Move to top-right"),
    _moveToBottomLeftAction(this, "Move to bottom-left"),
    _moveToBottomRightAction(this, "Move to bottom-right"),
    _learningCenterOverlayWidget(nullptr)
{
    setIconByName("circle-question");

    _viewHelpAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu, false);
    _viewHelpAction.setConnectionPermissionsToForceNone();

    connect(&_viewHelpAction, &TriggerAction::triggered, this, [this]() -> void {
        Q_ASSERT(_plugin);

        if (!_plugin)
            return;

        const_cast<plugin::PluginFactory*>(_plugin->getFactory())->getPluginMetadata().getTriggerHelpAction().trigger();
    });

    _toolbarVisibleAction.setToolTip("Toggle toolbar visibility");
    _toolbarVisibleAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu);
    _toolbarVisibleAction.setConnectionPermissionsToForceNone();

    _hideToolbarAction.setToolTip("Hide toolbar");
    _hideToolbarAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu);
    _hideToolbarAction.setConnectionPermissionsToForceNone();
    _hideToolbarAction.setIconByName("eye-slash");

    connect(&_hideToolbarAction, &TriggerAction::triggered, this, [this]() -> void { _toolbarVisibleAction.setChecked(false); });

    _alignmentAction.setToolTip("View plugin learning center toolbar alignment");
    _alignmentAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu);
    _alignmentAction.setConnectionPermissionsToForceNone();

    const auto pluginOverlayVisibleChanged = [this]() -> void {
        _toolbarVisibleAction.setIconByName(_toolbarVisibleAction.isChecked() ? "eye" : "eye-slash");
    };

    pluginOverlayVisibleChanged();

    connect(&_toolbarVisibleAction, &ToggleAction::toggled, this, pluginOverlayVisibleChanged);

    _moveToTopLeftAction.setIcon(getAlignmentIcon(Qt::AlignTop | Qt::AlignLeft));
    _moveToTopRightAction.setIcon(getAlignmentIcon(Qt::AlignTop | Qt::AlignRight));
    _moveToBottomLeftAction.setIcon(getAlignmentIcon(Qt::AlignBottom | Qt::AlignLeft));
    _moveToBottomRightAction.setIcon(getAlignmentIcon(Qt::AlignBottom | Qt::AlignRight));

    connect(&_moveToTopLeftAction, &TriggerAction::triggered, this, [this]() -> void { _alignmentAction.setCurrentText("TopLeft"); });
    connect(&_moveToTopRightAction, &TriggerAction::triggered, this, [this]() -> void { _alignmentAction.setCurrentText("TopRight"); });
    connect(&_moveToBottomLeftAction, &TriggerAction::triggered, this, [this]() -> void { _alignmentAction.setCurrentText("BottomLeft"); });
    connect(&_moveToBottomRightAction, &TriggerAction::triggered, this, [this]() -> void { _alignmentAction.setCurrentText("BottomRight"); });
}

void PluginLearningCenterAction::initialize(plugin::Plugin* plugin)
{
    Q_ASSERT(plugin);

    if (!plugin)
        return;

    _plugin = plugin;

    _viewHelpAction.setToolTip(QString("Shows %1 documentation").arg(_plugin->getKind()));

    if (_learningCenterOverlayWidget)
        _learningCenterOverlayWidget->deleteLater();
}

QMenu* PluginLearningCenterAction::getContextMenu(QWidget* parent)
{
    auto contextMenu = new QMenu("Learning center", parent);

    contextMenu->setIcon(StyledIcon("chalkboard-user"));

    contextMenu->addAction(&_toolbarVisibleAction);
    contextMenu->addMenu(getAlignmentContextMenu());

    return contextMenu;
}

QMenu* PluginLearningCenterAction::getAlignmentContextMenu(QWidget* parent)
{
    auto contextMenu = new QMenu("Alignment", parent);

    contextMenu->setEnabled(_toolbarVisibleAction.isChecked());
    contextMenu->setIcon(StyledIcon("arrows-alt"));

    if (getAlignment() != (Qt::AlignTop | Qt::AlignLeft))
        contextMenu->addAction(&_moveToTopLeftAction);

    if (getAlignment() != (Qt::AlignTop | Qt::AlignRight))
        contextMenu->addAction(&_moveToTopRightAction);

    if (getAlignment() != (Qt::AlignBottom | Qt::AlignLeft))
        contextMenu->addAction(&_moveToBottomLeftAction);

    if (getAlignment() != (Qt::AlignBottom | Qt::AlignRight))
        contextMenu->addAction(&_moveToBottomRightAction);

    return contextMenu;
}

ViewPluginLearningCenterOverlayWidget* PluginLearningCenterAction::getViewPluginOverlayWidget() const
{
    return _learningCenterOverlayWidget;
}

Qt::Alignment PluginLearningCenterAction::getAlignment() const
{
    const auto currentAlignmentText = _alignmentAction.getCurrentText();

    if (!alignmentOptions.contains(currentAlignmentText))
        return Qt::AlignCenter;

    return alignmentFlags[alignmentOptions.indexOf(currentAlignmentText)];
}

void PluginLearningCenterAction::setAlignment(const Qt::Alignment& alignment)
{
	const auto it = std::find(alignmentFlags.begin(), alignmentFlags.end(), alignment);

    if (it != alignmentFlags.end())
        _alignmentAction.setCurrentIndex(static_cast<std::int32_t>(std::distance(alignmentFlags.begin(), it)));
}

plugin::PluginMetadata& PluginLearningCenterAction::getPluginMetadata()
{
    return const_cast<PluginFactory*>(getViewPlugin()->getFactory())->getPluginMetadata();
}

const plugin::PluginMetadata& PluginLearningCenterAction::getPluginMetaData() const
{
    return getViewPlugin()->getFactory()->getPluginMetadata();
}

void PluginLearningCenterAction::createViewPluginOverlayWidget()
{
    _learningCenterOverlayWidget = new ViewPluginLearningCenterOverlayWidget(&getViewPlugin()->getWidget(), getViewPlugin());
    _learningCenterOverlayWidget->show();
}

bool PluginLearningCenterAction::hasHelp() const
{
    return _plugin->getFactory()->hasHelp();
}

void PluginLearningCenterAction::addVideo(const util::LearningCenterVideo* video)
{
    _videos.push_back(video);
}

void PluginLearningCenterAction::addVideos(const util::LearningCenterVideos& videos)
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

const LearningCenterVideos& PluginLearningCenterAction::getVideos() const
{
    return _videos;
}

void PluginLearningCenterAction::addTutorial(const util::LearningCenterTutorial* tutorial)
{
    _tutorials.push_back(tutorial);
}

void PluginLearningCenterAction::addTutorials(const util::LearningCenterTutorials& tutorials)
{
    _tutorials.insert(_tutorials.end(), tutorials.begin(), tutorials.end());
}

void PluginLearningCenterAction::addTutorials(const QString& tag)
{
    addTutorials(mv::help().getTutorials({ tag }));
}

void PluginLearningCenterAction::addTutorials(const QStringList& tags)
{
    addTutorials(mv::help().getTutorials(tags));
}

const util::LearningCenterTutorials& PluginLearningCenterAction::getTutorials() const
{
    return _tutorials;
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

void PluginLearningCenterAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _toolbarVisibleAction.fromParentVariantMap(variantMap);
    _alignmentAction.fromParentVariantMap(variantMap);
}

QVariantMap PluginLearningCenterAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    _toolbarVisibleAction.insertIntoVariantMap(variantMap);
    _alignmentAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
