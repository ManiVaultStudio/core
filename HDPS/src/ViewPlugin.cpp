// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPlugin.h"
#include "CoreInterface.h"
#include "Application.h"
#include "AbstractWorkspaceManager.h"

#include "widgets/ViewPluginEditorDialog.h"

#include <QWidget>
#include <QFileDialog>

#ifdef _DEBUG
    #define VIEW_PLUGIN_VERBOSE
#endif

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps::plugin
{

ViewPlugin::ViewPlugin(const PluginFactory* factory) :
    Plugin(factory),
    _widget(),
    _editorAction(&_widget, "Edit..."),
    _screenshotAction(&_widget, "Screenshot..."),
    _isolateAction(&_widget, "Isolate"),
    _mayCloseAction(this, "May close", true),
    _mayFloatAction(this, "May float", true),
    _mayMoveAction(this, "May move", true),
    _dockingOptionsAction(this, "Docking options", { "May Close", "May Float", "May Move" }),
    _lockingAction(this),
    _visibleAction(this, "Visible", true),
    _helpAction(this, "Trigger help"),
    _presetsAction(this, this, QString("%1/Presets").arg(getKind()), getKind(), factory->getIcon()),
    _triggerShortcut(),
    _titleBarMenuActions()
{
    setText(getGuiName());

    _widget.setAutoFillBackground(true);

    _widget.addAction(&_editorAction);
    _widget.addAction(&_screenshotAction);
    _widget.addAction(&_isolateAction);
    _widget.addAction(&_helpAction);

    _editorAction.setIconByName("cog");
    _editorAction.setShortcut(tr("F12"));
    _editorAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _editorAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _editorAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);
    _editorAction.setConnectionPermissionsToForceNone();

    _screenshotAction.setIconByName("camera");
    _screenshotAction.setShortcut(tr("F2"));
    _screenshotAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _screenshotAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _screenshotAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);
    _screenshotAction.setConnectionPermissionsToForceNone();

    _isolateAction.setIconByName("crosshairs");
    _isolateAction.setShortcut(tr("F3"));
    _isolateAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _isolateAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _isolateAction.setConnectionPermissionsToForceNone();

    _mayCloseAction.setToolTip("Determines whether this view plugin may be closed or not");
    _mayCloseAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _mayCloseAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);
    _mayCloseAction.setConnectionPermissionsToForceNone();

    _mayFloatAction.setToolTip("Determines whether this view plugin may float or not");
    _mayFloatAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _mayFloatAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);
    _mayFloatAction.setConnectionPermissionsToForceNone();

    _mayMoveAction.setToolTip("Determines whether this view plugin may be moved or not");
    _mayMoveAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _mayMoveAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);
    _mayMoveAction.setConnectionPermissionsToForceNone();

    _dockingOptionsAction.setToolTip("Determines the docking options");
    _dockingOptionsAction.setConnectionPermissionsToForceNone(true);

    _lockingAction.setWhat("Layout");

    _visibleAction.setToolTip("Determines whether the view plugin is visible or not");
    _visibleAction.setIcon(getIcon());
    _visibleAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _visibleAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);

    _helpAction.setToolTip(QString("Shows %1 documentation").arg(factory->getKind()));
    _helpAction.setShortcut(tr("F1"));
    _helpAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _helpAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu, false);
    _helpAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly, false);
    _helpAction.setConnectionPermissionsToForceNone();

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

    connect(&_helpAction, &TriggerAction::triggered, this, [this]() -> void {
        getTriggerHelpAction().trigger();
    });

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
    auto* fileDialog = new QFileDialog();

    fileDialog->setWindowTitle("Save Screenshot");
    fileDialog->setWindowIcon(Application::getIconFont("FontAwesome").getIcon("camera"));
    fileDialog->setOption(QFileDialog::DontUseNativeDialog, true);
    fileDialog->setAcceptMode(QFileDialog::AcceptSave);
    fileDialog->setNameFilters({ "Image files (*.png)" });
    fileDialog->setDefaultSuffix(".png");

    const auto cachedDirectory = QFileInfo(Application::current()->getSetting("ScreenShot/ViewPlugin/Directory", ".").toString()).dir();

    fileDialog->setDirectory(cachedDirectory);

    connect(fileDialog, &QFileDialog::accepted, this, [this, fileDialog]() -> void {
        if (fileDialog->selectedFiles().count() != 1)
            throw std::runtime_error("Only one file may be selected");

        Application::current()->setSetting("ScreenShot/ViewPlugin/Directory", fileDialog->selectedFiles().first());

        auto widgetPixmap = getWidget().grab();

        widgetPixmap.toImage().save(fileDialog->selectedFiles().first());
        });
    connect(fileDialog, &QFileDialog::finished, fileDialog, &QFileDialog::deleteLater);

    fileDialog->open();
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
}

QVariantMap ViewPlugin::toVariantMap() const
{
    auto variantMap = Plugin::toVariantMap();

    _mayCloseAction.insertIntoVariantMap(variantMap);
    _mayFloatAction.insertIntoVariantMap(variantMap);
    _mayMoveAction.insertIntoVariantMap(variantMap);
    _lockingAction.insertIntoVariantMap(variantMap);
    _visibleAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

void ViewPlugin::addTitleBarMenuAction(hdps::gui::WidgetAction* action)
{
    Q_ASSERT(action != nullptr);

    if (action == nullptr)
        return;

    _titleBarMenuActions << action;
}

void ViewPlugin::removeTitleBarMenuAction(hdps::gui::WidgetAction* action)
{
    Q_ASSERT(action != nullptr);

    if (action == nullptr)
        return;

    _titleBarMenuActions.removeOne(action);
}

hdps::gui::WidgetActions ViewPlugin::getTitleBarMenuActions()
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
}

hdps::gui::WidgetActions ViewPlugin::getDockingActions() const
{
    return _settingsActions;
}

ViewPluginFactory::ViewPluginFactory(bool producesSystemViewPlugins /*= false*/) :
    PluginFactory(Type::VIEW),
    _producesSystemViewPlugins(producesSystemViewPlugins),
    _preferredDockArea(DockAreaFlag::Right)
{
    if (_producesSystemViewPlugins)
        setMaximumNumberOfInstances(1);
}

QIcon ViewPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("eye", color);
}

bool ViewPluginFactory::producesSystemViewPlugins() const
{
    return _producesSystemViewPlugins;
}

DockAreaFlag ViewPluginFactory::getPreferredDockArea() const
{
    return _preferredDockArea;
}

void ViewPluginFactory::setPreferredDockArea(const gui::DockAreaFlag& preferredDockArea)
{
    _preferredDockArea = preferredDockArea;
}

}
