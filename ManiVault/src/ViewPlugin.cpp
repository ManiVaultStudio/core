// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPlugin.h"
#include "CoreInterface.h"
#include "Application.h"
#include "AbstractWorkspaceManager.h"

#include "widgets/ViewPluginEditorDialog.h"
#include "widgets/FileDialog.h"
#include "widgets/ActionOverlayWidget.h"
#include "widgets/ViewPluginOverlayWidget.h"

#include <QWidget>

#include <ranges>

#ifdef _DEBUG
    #define VIEW_PLUGIN_VERBOSE
#endif

using namespace mv::gui;
using namespace mv::util;

namespace mv::plugin
{

ViewPlugin::ViewPlugin(const PluginFactory* factory) :
    Plugin(factory),
    _editorAction(this, "Edit..."),
    _screenshotAction(this, "Screenshot..."),
    _isolateAction(this, "Isolate"),
    _mayCloseAction(this, "May close", true),
    _mayFloatAction(this, "May float", true),
    _mayMoveAction(this, "May move", true),
    _dockingOptionsAction(this, "Docking options", { "May Close", "May Float", "May Move" }),
    _lockingAction(this),
    _visibleAction(this, "Visible", true),
    _presetsAction(this, this, QString("%1/Presets").arg(getKind()), getKind(), StyledIcon(factory->icon())),
    _samplerAction(this, "Sampler"),
    _progressTask(nullptr),
    _headsUpDisplayAction(this, "HUD")
{
    setText(isSystemViewPlugin() ? getKind() : getGuiName());

    _widget.setAutoFillBackground(true);

    _widget.addAction(&_editorAction);
    _widget.addAction(&_screenshotAction);
    _widget.addAction(&_isolateAction);

    _editorAction.setIconByName("gear");
    _editorAction.setShortcut(tr("F12"));
    _editorAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _editorAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu);
    _editorAction.setConnectionPermissionsToForceNone();

    _screenshotAction.setIconByName("camera");
    _screenshotAction.setShortcut(tr("F2"));
    _screenshotAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _screenshotAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu);
    _screenshotAction.setConnectionPermissionsToForceNone();

    _isolateAction.setIconByName("crosshairs");
    _isolateAction.setShortcut(tr("F3"));
    _isolateAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _isolateAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu);
    _isolateAction.setConnectionPermissionsToForceNone();

    _mayCloseAction.setToolTip("Determines whether this view plugin may be closed or not");
    _mayCloseAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu);
    _mayCloseAction.setConnectionPermissionsToForceNone();

    _mayFloatAction.setToolTip("Determines whether this view plugin may float or not");
    _mayFloatAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu);
    _mayFloatAction.setConnectionPermissionsToForceNone();

    _mayMoveAction.setToolTip("Determines whether this view plugin may be moved or not");
    _mayMoveAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu);
    _mayMoveAction.setConnectionPermissionsToForceNone();

    _dockingOptionsAction.setToolTip("Determines the docking options");
    _dockingOptionsAction.setConnectionPermissionsToForceNone(true);

    _lockingAction.setWhat("Layout");

    _visibleAction.setToolTip("Determines whether the view plugin is visible or not");
    _visibleAction.setIcon(StyledIcon(icon()));
    _visibleAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu);

    _samplerAction.setToolTip(QStringLiteral("Element sampler"));
    _samplerAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _samplerAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu, false);
    _samplerAction.setConnectionPermissionsToForceNone();

    connect(&_editorAction, &TriggerAction::triggered, this, [this]() -> void {
        auto* viewPluginEditorDialog = new ViewPluginEditorDialog(nullptr, this);
        connect(viewPluginEditorDialog, &ViewPluginEditorDialog::finished, viewPluginEditorDialog, &ViewPluginEditorDialog::deleteLater);
        viewPluginEditorDialog->open();
    });

    connect(&_screenshotAction, &TriggerAction::triggered, this, [this]() -> void {
        createScreenshot();
    });

    connect(&_isolateAction, &ToggleAction::toggled, this, [this](bool toggled) -> void {
        workspaces().isolateViewPlugin(this, toggled);
    });

    const auto updateDockWidgetPermissionsReadOnly = [this]() -> void {
        const auto enabled = !_lockingAction.getLockedAction().isChecked();

        _mayCloseAction.setEnabled(enabled);
        _mayFloatAction.setEnabled(enabled);
        _mayMoveAction.setEnabled(enabled);
    };

    connect(&_lockingAction.getLockedAction(), &ToggleAction::toggled, this, [this, updateDockWidgetPermissionsReadOnly](bool toggled) -> void {
        _mayCloseAction.setChecked(!toggled);
        _mayFloatAction.setChecked(!toggled);
        _mayMoveAction.setChecked(!toggled);

        getDestroyAction().setEnabled(!toggled);

        _dockingOptionsAction.setEnabled(!toggled);

        updateDockWidgetPermissionsReadOnly();
    });

    updateDockWidgetPermissionsReadOnly();

    connect(&getGuiNameAction(), &StringAction::stringChanged, this, [this](const QString& guiName) -> void {
        _widget.setWindowTitle(guiName);
    });

    const auto updateVisibleAction = [this]() -> void {
        _visibleAction.setEnabled(_mayCloseAction.isChecked());
    };

    connect(&_mayCloseAction, &ToggleAction::toggled, this, updateVisibleAction);

    //_visibleAction.setText(getGuiName());

    updateVisibleAction();

    const auto updateDockingOptionsAction = [this]() -> void {
        QStringList selectedOptions;
        
        if (_mayCloseAction.isChecked())
            selectedOptions << "May Close";

        if (_mayFloatAction.isChecked())
            selectedOptions << "May Float";

        if (_mayMoveAction.isChecked())
            selectedOptions << "May Move";

        _dockingOptionsAction.setSelectedOptions(selectedOptions);
    };

    updateDockingOptionsAction();

    connect(&_mayCloseAction, &ToggleAction::toggled, this, updateDockingOptionsAction);
    connect(&_mayFloatAction, &ToggleAction::toggled, this, updateDockingOptionsAction);
    connect(&_mayMoveAction, &ToggleAction::toggled, this, updateDockingOptionsAction);

    connect(&_dockingOptionsAction, &OptionsAction::selectedOptionsChanged, this, [this]() -> void {
        _mayCloseAction.setChecked(_dockingOptionsAction.getSelectedOptions().contains("May Close"));
        _mayFloatAction.setChecked(_dockingOptionsAction.getSelectedOptions().contains("May Float"));
        _mayMoveAction.setChecked(_dockingOptionsAction.getSelectedOptions().contains("May Move"));
    });

    auto& shortcuts = getShortcuts();

    shortcuts.add({ QKeySequence(Qt::Key_F2), "General", "Make a screenshot" });

    if (!isSystemViewPlugin())
        shortcuts.add({ QKeySequence(Qt::Key_F3), "General", "Toggle view isolation (hides the other views)" });

    shortcuts.add({ QKeySequence(Qt::Key_F12), "General", "Edit the view parameters" });

    getLearningCenterAction().createViewPluginOverlayWidget();

    addTitleBarMenuAction(&getLearningCenterAction());
    addOverlayAction(&_headsUpDisplayAction);
}

void ViewPlugin::init()
{
}

void ViewPlugin::loadData(const Datasets& datasets)
{
    qDebug() << "Load function not implemented in view plugin implementation";
}

QWidget& ViewPlugin::getWidget()
{
    return _widget;
}

bool ViewPlugin::isSystemViewPlugin() const
{
    return dynamic_cast<const ViewPluginFactory*>(_factory)->producesSystemViewPlugins();
}

void ViewPlugin::createScreenshot()
{
    auto fileSaveDialog = new FileSaveDialog();

    fileSaveDialog->setWindowTitle("Save Screenshot");
    fileSaveDialog->setNameFilters({ "Image files (*.png)" });
    fileSaveDialog->setDefaultSuffix(".png");

    const auto cachedDirectory = QFileInfo(Application::current()->getSetting("ScreenShot/ViewPlugin/Directory", ".").toString()).dir();

    fileSaveDialog->setDirectory(cachedDirectory);

    connect(fileSaveDialog, &QFileDialog::accepted, this, [this, fileSaveDialog]() -> void {
        if (fileSaveDialog->selectedFiles().count() != 1)
            throw std::runtime_error("Only one file may be selected");

        Application::current()->setSetting("ScreenShot/ViewPlugin/Directory", fileSaveDialog->selectedFiles().first());

        auto widgetPixmap = getWidget().grab();

        widgetPixmap.toImage().save(fileSaveDialog->selectedFiles().first());
    });

    connect(fileSaveDialog, &QFileDialog::finished, fileSaveDialog, &QFileDialog::deleteLater);

    fileSaveDialog->open();
}

QKeySequence ViewPlugin::getTriggerShortcut() const
{
    return _triggerShortcut;
}

void ViewPlugin::setTriggerShortcut(const QKeySequence& keySequence)
{
    _triggerShortcut = keySequence;
}

void ViewPlugin::fromVariantMap(const QVariantMap& variantMap)
{
    Plugin::fromVariantMap(variantMap);

    _mayCloseAction.fromParentVariantMap(variantMap);
    _mayFloatAction.fromParentVariantMap(variantMap);
    _mayMoveAction.fromParentVariantMap(variantMap);
    _lockingAction.fromParentVariantMap(variantMap);
    _visibleAction.fromParentVariantMap(variantMap);
    _samplerAction.fromParentVariantMap(variantMap);
}

QVariantMap ViewPlugin::toVariantMap() const
{
    auto variantMap = Plugin::toVariantMap();

    _mayCloseAction.insertIntoVariantMap(variantMap);
    _mayFloatAction.insertIntoVariantMap(variantMap);
    _mayMoveAction.insertIntoVariantMap(variantMap);
    _lockingAction.insertIntoVariantMap(variantMap);
    _visibleAction.insertIntoVariantMap(variantMap);
    _samplerAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

void ViewPlugin::addTitleBarMenuAction(mv::gui::WidgetAction* action)
{
    Q_ASSERT(action != nullptr);

    if (action == nullptr)
        return;

    _titleBarMenuActions << action;
}

void ViewPlugin::removeTitleBarMenuAction(mv::gui::WidgetAction* action)
{
    Q_ASSERT(action != nullptr);

    if (action == nullptr)
        return;

    _titleBarMenuActions.removeOne(action);
}

mv::gui::WidgetActions ViewPlugin::getTitleBarMenuActions()
{
    return _titleBarMenuActions;
}

void ViewPlugin::addDockingAction(WidgetAction* dockingAction, WidgetAction* dockToDockingAction /*= nullptr*/, gui::DockAreaFlag dockArea /*= gui::DockAreaFlag::Right*/, bool autoHide /*= false*/, const gui::AutoHideLocation& autoHideLocation /*= gui::AutoHideLocation::Left*/, const QSize& minimumDockWidgetSize /*= QSize(256, 256)*/)
{
    Q_ASSERT(dockingAction != nullptr);

    if (dockingAction == nullptr)
        return;

    dockingAction->setProperty("DockToDockingActionName", dockToDockingAction != nullptr ? dockToDockingAction->text() : "");
    dockingAction->setProperty("DockArea", static_cast<int>(dockArea));
    dockingAction->setProperty("AutoHide", autoHide);
    dockingAction->setProperty("AutoHideLocation", static_cast<int>(autoHideLocation));
    dockingAction->setProperty("MinimumDockWidgetSize", minimumDockWidgetSize);

    _settingsActions << dockingAction;

    emit dockingActionAdded(dockingAction);
}

mv::gui::WidgetActions ViewPlugin::getDockingActions() const
{
    return _settingsActions;
}

void ViewPlugin::addOverlayAction(WidgetAction* overlayAction, const Qt::Alignment& alignment)
{
    Q_ASSERT(overlayAction);

    if (overlayAction == nullptr)
        return;

    _actionsWidgets.push_back({ overlayAction, new ActionOverlayWidget(&_widget, overlayAction, alignment) });

    connect(overlayAction, &WidgetAction::destroyed, this, [this, overlayAction]() -> void {
        removeOverlayAction(overlayAction);
    });
}

void ViewPlugin::removeOverlayAction(WidgetAction* overlayAction)
{
    Q_ASSERT(overlayAction);

    if (overlayAction == nullptr)
        return;

    auto it = std::remove_if(_actionsWidgets.begin(), _actionsWidgets.end(),
        [overlayAction](const ActionWidgetPair& p) {
            return p.first == overlayAction;
    });

    if (it != _actionsWidgets.end()) {
        it->second->deleteLater();

        _actionsWidgets.erase(it);
    }
}

WidgetActions ViewPlugin::getOverlayActions() const
{
    WidgetActions overlayActions;

    auto actions = _actionsWidgets | std::views::transform([](auto& p) { return p.first; });

    for (const auto& action : actions)
        overlayActions << action;

    return overlayActions;
}

void ViewPlugin::setOverlayActionsTargetWidget(QWidget* targetWidget)
{
    Q_ASSERT(targetWidget);

    if (targetWidget == nullptr)
        return;

    for (const auto& actionWidgetPair : _actionsWidgets)
        actionWidgetPair.second->setTargetWidget(targetWidget);
}

mv::Task* ViewPlugin::getProgressTask()
{
    return _progressTask;
}

void ViewPlugin::setProgressTask(Task* progressTask)
{
    if (progressTask == _progressTask)
        return;

    _progressTask = progressTask;

    emit progressTaskChanged(_progressTask);
}

ViewPluginFactory::ViewPluginFactory(bool producesSystemViewPlugins /*= false*/, bool startFloating /*= false*/) :
    PluginFactory(Type::VIEW, "View"),
    _producesSystemViewPlugins(producesSystemViewPlugins),
    _preferredDockArea(DockAreaFlag::Right),
    _startFloating(startFloating)
{
    setIconByName("eye");
    setCategoryIconByName("eye");

    if (_producesSystemViewPlugins)
        setMaximumNumberOfInstances(1);
}

bool ViewPluginFactory::producesSystemViewPlugins() const
{
    return _producesSystemViewPlugins;
}

DockAreaFlag ViewPluginFactory::getPreferredDockArea() const
{
    return _preferredDockArea;
}

bool ViewPluginFactory::getStartFloating() const
{
    return _startFloating;
}

void ViewPluginFactory::setPreferredDockArea(const gui::DockAreaFlag& preferredDockArea)
{
    _preferredDockArea = preferredDockArea;
}

}
