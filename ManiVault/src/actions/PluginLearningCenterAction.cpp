// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginLearningCenterAction.h"

#include "CoreInterface.h"
#include "Plugin.h"
#include "ViewPlugin.h"

#include "util/Miscellaneous.h"

#include "widgets/ViewPluginLearningCenterOverlayWidget.h"
#include "widgets/ViewPluginDescriptionOverlayWidget.h"
#include "widgets/ViewPluginShortcutsDialog.h"

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
    _viewDescriptionAction(this, "View description"),
    _viewHelpAction(this, "View help"),
    _viewShortcutsAction(this, "View shortcuts"),
    _toolbarVisibleAction(this, "Visible", true),
    _hideToolbarAction(this, "Hide toolbar"),
    _alignmentAction(this, "View plugin overlay alignment", alignmentOptions, "BottomLeft"),
    _moveToTopLeftAction(this, "Move to top-left"),
    _moveToTopRightAction(this, "Move to top-right"),
    _moveToBottomLeftAction(this, "Move to bottom-left"),
    _moveToBottomRightAction(this, "Move to bottom-right"),
    _learningCenterOverlayWidget(nullptr)
{
    setIconByName("question-circle");

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
        _alignmentAction.setEnabled(_toolbarVisibleAction.isChecked());
    };

    pluginOverlayVisibleChanged();

    connect(&_toolbarVisibleAction, &ToggleAction::toggled, this, pluginOverlayVisibleChanged);

    _moveToTopLeftAction.setIcon(getAlignmentIcon(Qt::AlignTop | Qt::AlignLeft));
    _moveToTopRightAction.setIcon(getAlignmentIcon(Qt::AlignTop | Qt::AlignRight));
    _moveToBottomLeftAction.setIcon(getAlignmentIcon(Qt::AlignBottom | Qt::AlignLeft));
    _moveToBottomRightAction.setIcon(getAlignmentIcon(Qt::AlignBottom | Qt::AlignLeft));

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

    if (_shortDescription.isEmpty())
        setShortDescription(QString("View %1 description").arg(_plugin->getKind()));

    if (_longDescription.isEmpty())
        setShortDescription("No description available yet, stay tuned");

    if (_learningCenterOverlayWidget)
        _learningCenterOverlayWidget->deleteLater();
}

QMenu* PluginLearningCenterAction::getContextMenu(QWidget* parent)
{
    auto contextMenu = new QMenu("Learning center", parent);

    contextMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("chalkboard-teacher"));

    contextMenu->addAction(&_toolbarVisibleAction);
    contextMenu->addMenu(getAlignmentContextMenu());

    return contextMenu;
}

QMenu* PluginLearningCenterAction::getAlignmentContextMenu(QWidget* parent)
{
    auto contextMenu = new QMenu("Alignment", parent);

    contextMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("arrows-alt"));

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
        _alignmentAction.setCurrentIndex(std::distance(alignmentFlags.begin(), it));
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
        ViewPluginShortcutsDialog viewPluginShortcutsDialog(dynamic_cast<ViewPlugin*>(_plugin));

        viewPluginShortcutsDialog.exec();
    }
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
