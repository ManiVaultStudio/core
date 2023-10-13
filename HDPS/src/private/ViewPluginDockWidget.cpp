// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginDockWidget.h"
#include "DockManager.h"

#include <Application.h>
#include <CoreInterface.h>

#include <actions/WidgetAction.h>
#include <actions/ToolbarAction.h>

#include <ViewPlugin.h>

#include <util/Serialization.h>

#include <DockWidgetTab.h>
#include <DockAreaWidget.h>
#include <DockAreaTitleBar.h>
#include <AutoHideDockContainer.h>

#ifdef _DEBUG
    //#define VIEW_PLUGIN_DOCK_WIDGET_VERBOSE
#endif

using namespace ads;

using namespace hdps;
using namespace hdps::plugin;
using namespace hdps::util;
using namespace hdps::gui;

QList<ViewPluginDockWidget*> ViewPluginDockWidget::active = QList<ViewPluginDockWidget*>();

QMap<QString, Task*> ViewPluginDockWidget::serializationTasks = QMap<QString, Task*>();

ViewPluginDockWidget::ViewPluginDockWidget(const QString& title /*= ""*/, QWidget* parent /*= nullptr*/) :
    DockWidget(title, parent),
    _viewPlugin(nullptr),
    _viewPluginKind(),
    _viewPluginMap(),
    _settingsMenu(),
    _toggleMenu("Toggle"),
    _helpAction(this, "Help"),
    _cachedVisibility(false),
    _dockManager(this),
    _settingsDockWidgetsMap()
{
    active << this;

    setFeature(CDockWidget::DockWidgetDeleteOnClose, false);
    initialize();
}

ViewPluginDockWidget::ViewPluginDockWidget(const QString& title, ViewPlugin* viewPlugin, QWidget* parent /*= nullptr*/) :
    DockWidget(title, parent),
    _viewPlugin(nullptr),
    _viewPluginKind(),
    _viewPluginMap(),
    _settingsMenu(),
    _toggleMenu("Toggle"),
    _helpAction(this, "Help"),
    _cachedVisibility(false),
    _dockManager(this),
    _settingsDockWidgetsMap()
{
    active << this;

    setFeature(CDockWidget::DockWidgetDeleteOnClose, false);
    initialize();
    setViewPlugin(viewPlugin);
}

ViewPluginDockWidget::ViewPluginDockWidget(const QVariantMap& variantMap) :
    DockWidget(""),
    _viewPlugin(nullptr),
    _viewPluginKind(),
    _viewPluginMap(),
    _settingsMenu(),
    _toggleMenu("Toggle"),
    _helpAction(this, "Help"),
    _cachedVisibility(false),
    _dockManager(this),
    _settingsDockWidgetsMap()
{
    active << this;

    setFeature(CDockWidget::DockWidgetDeleteOnClose, false);
    initialize();
    fromVariantMap(variantMap);
}

ViewPluginDockWidget::~ViewPluginDockWidget()
{
#ifdef VIEW_PLUGIN_DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__ << windowTitle();
#endif

    serializationTasks.remove(getId());
}

QString ViewPluginDockWidget::getTypeString() const
{
    return "ViewPluginDockWidget";
}

void ViewPluginDockWidget::initialize()
{
    _toggleMenu.setIcon(Application::getIconFont("FontAwesome").getIcon("low-vision"));

    _helpAction.setIconByName("question");
    _helpAction.setShortcut(tr("F1"));
    _helpAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _helpAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _helpAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);

    connect(&_helpAction, &TriggerAction::triggered, this, [this]() -> void {
        _viewPlugin->getTriggerHelpAction().trigger();
    });

    connect(&plugins(), &AbstractPluginManager::pluginAboutToBeDestroyed, this, [this](plugin::Plugin* plugin) -> void {
        if (plugin != _viewPlugin)
            return;

#ifdef VIEW_PLUGIN_DOCK_WIDGET_VERBOSE
        qDebug() << "Remove active view plugin dock widget" << plugin->getGuiName();
#endif

        active.removeOne(this);

        takeWidget();
    });

    connect(&_settingsMenu, &QMenu::aboutToShow, this, [this]() -> void {
        _settingsMenu.clear();
        
        const auto projectIsReadOnly = projects().hasProject() ? projects().getCurrentProject()->getReadOnlyAction().isChecked() : false;

        if (!_viewPlugin->isSystemViewPlugin() && !projectIsReadOnly) {
            _settingsMenu.addMenu(_viewPlugin->getPresetsAction().getMenu());
            _settingsMenu.addSeparator();
        }

        if (_viewPlugin->hasHelp())
            _settingsMenu.addAction(&_helpAction);

        _settingsMenu.addAction(&_viewPlugin->getScreenshotAction());

        if (!_viewPlugin->isSystemViewPlugin())
            _settingsMenu.addAction(&_viewPlugin->getIsolateAction());

        _settingsMenu.addSeparator();

        if (!projectIsReadOnly)
            _settingsMenu.addAction(&_viewPlugin->getDestroyAction());

        if (!_viewPlugin->getLockingAction().isLocked() && !projectIsReadOnly)
            _settingsMenu.addAction(&_viewPlugin->getEditorAction());

        _settingsMenu.addSeparator();

        if (!projectIsReadOnly)
            _settingsMenu.addAction(&_viewPlugin->getLockingAction().getLockedAction());

        _settingsMenu.addSeparator();

        if (projects().getCurrentProject()->getStudioModeAction().isChecked())
            _settingsMenu.addMenu(&_toggleMenu);

        _settingsMenu.addSeparator();

        if (!_viewPlugin->getTitleBarMenuActions().isEmpty()) {
            _settingsMenu.addSeparator();

            for (auto titleBarMenuAction : _viewPlugin->getTitleBarMenuActions())
                _settingsMenu.addAction(titleBarMenuAction);
        }
    });
}

void ViewPluginDockWidget::loadViewPlugin()
{
#ifdef VIEW_PLUGIN_DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (!plugins().isPluginLoaded(_viewPluginKind))
        return;

    auto viewPlugin = dynamic_cast<ViewPlugin*>(plugins().requestPlugin(_viewPluginKind));

    if (viewPlugin)
        setViewPlugin(viewPlugin);
}

ViewPlugin* ViewPluginDockWidget::getViewPlugin()
{
    return _viewPlugin;
}

void ViewPluginDockWidget::restoreViewPluginState()
{
    if (!_viewPlugin)
        return;

    _viewPlugin->fromVariantMap(_viewPluginMap);
}
void ViewPluginDockWidget::restoreViewPluginStates()
{
    for (auto viewPluginDockWidget : ViewPluginDockWidget::active)
        viewPluginDockWidget->restoreViewPluginState();
}

QMenu* ViewPluginDockWidget::getSettingsMenu()
{
    return &_settingsMenu;
}

void ViewPluginDockWidget::fromVariantMap(const QVariantMap& variantMap)
{
#ifdef VIEW_PLUGIN_DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    DockWidget::fromVariantMap(variantMap);

    const auto viewPluginDockWidgetId   = variantMap["ID"].toString();
    const auto viewPluginMap            = variantMap["ViewPlugin"].toMap();
    const auto guiName                  = viewPluginMap["GuiName"].toMap()["Value"].toString();

    auto serializationTask = ViewPluginDockWidget::getSerializationTask(viewPluginDockWidgetId);

    serializationTask->setName(QString("Loading view plugin: %1").arg(guiName));
    serializationTask->setRunning();

    variantMapMustContain(variantMap, "ViewPlugin");

    _viewPluginMap = variantMap["ViewPlugin"].toMap();

    variantMapMustContain(_viewPluginMap, "Kind");

    _viewPluginKind = _viewPluginMap["Kind"].toString();

    loadViewPlugin();

    QCoreApplication::processEvents();

    if (variantMap.contains("DockManagerState"))
        _dockManager.restoreState(QByteArray::fromBase64(variantMap["DockManagerState"].toString().toUtf8()));

    QCoreApplication::processEvents();

    serializationTask->setFinished();
}

QVariantMap ViewPluginDockWidget::toVariantMap() const
{
#ifdef VIEW_PLUGIN_DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    auto viewPlugin         = const_cast<ViewPluginDockWidget*>(this)->getViewPlugin();
    auto serializationTask  = ViewPluginDockWidget::getSerializationTask(getId());

    serializationTask->setName(QString("Saving view plugin: %1").arg(viewPlugin->text()));
    serializationTask->setRunning();

    auto variantMap = DockWidget::toVariantMap();

    if (_viewPlugin) {
        variantMap.insert({
            { "ViewPlugin", viewPlugin->toVariantMap() }
        });
    }
    
    QCoreApplication::processEvents();

    variantMap.insert({
        { "DockManagerState", QVariant::fromValue(_dockManager.saveState().toBase64()) }
    });

    serializationTask->setFinished();

    return variantMap;
}

void ViewPluginDockWidget::cacheVisibility()
{
    _cachedVisibility = !isClosed();
}

void ViewPluginDockWidget::restoreVisibility()
{
    toggleView(_cachedVisibility);
}

void ViewPluginDockWidget::preRegisterSerializationTask(QObject* parent, const QString& viewPluginDockWidgetId, DockManager* dockManager)
{
    if (viewPluginDockWidgetId.isEmpty())
        return;

    auto serializationTask = new Task(parent, "View plugin Dock Widget");

    serializationTask->setMayKill(false);
    serializationTask->setParentTask(dockManager->getSerializationTask());

    serializationTasks[viewPluginDockWidgetId] = serializationTask;
}

Task* ViewPluginDockWidget::getSerializationTask(const QString& viewPluginDockWidgetId)
{
    if (serializationTasks.contains(viewPluginDockWidgetId))
        return serializationTasks[viewPluginDockWidgetId];

    return nullptr;
}

void ViewPluginDockWidget::removeAllSerializationTasks()
{
    for (auto serializationTask : serializationTasks.values())
        delete serializationTask;

    serializationTasks.clear();
}

void ViewPluginDockWidget::setViewPlugin(hdps::plugin::ViewPlugin* viewPlugin)
{
#ifdef VIEW_PLUGIN_DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    Q_ASSERT(viewPlugin != nullptr);

    if (!viewPlugin)
        return;

    _viewPlugin = viewPlugin;

    setProperty("ViewPluginId", _viewPlugin->getId());

    auto centralDockWidget = new CDockWidget("Central");

    centralDockWidget->setWidget(&_viewPlugin->getWidget(), eInsertMode::ForceNoScrollArea);

    _dockManager.setCentralWidget(centralDockWidget);

    centralDockWidget->dockAreaWidget()->setAllowedAreas(DockWidgetArea::NoDockWidgetArea);

    auto hideAllAction = new TriggerAction(this, "Hide All");
    auto showAllAction = new TriggerAction(this, "Show All");

    hideAllAction->setEnabled(true);
    showAllAction->setEnabled(false);

    const auto updateHideShowAllActionsReadOnly = [this, hideAllAction, showAllAction]() -> void {
        const auto settingsDockWidgets                  = _settingsDockWidgetsMap.values();
        const auto numberOfSettingsDockWidgets          = _settingsDockWidgetsMap.count();
        const auto numberOfVisibleSettingsDockWidgets   = std::count_if(settingsDockWidgets.begin(), settingsDockWidgets.end(), [](CDockWidget* settingsDockWidget) { return settingsDockWidget->isVisible(); });

        hideAllAction->setEnabled(numberOfVisibleSettingsDockWidgets >= 1);
        showAllAction->setEnabled(numberOfVisibleSettingsDockWidgets < numberOfSettingsDockWidgets);
    };

    for (auto settingsAction : _viewPlugin->getDockingActions()) {
        auto settingsDockWidget = new CDockWidget(settingsAction->text());
        auto settingsWidget     = new SettingsActionWidget(this, settingsAction);

        settingsDockWidget->setWidget(settingsWidget, eInsertMode::ForceNoScrollArea);
        settingsDockWidget->setAutoFillBackground(true);
        settingsDockWidget->setFeature(CDockWidget::DockWidgetFloatable, false);
        settingsDockWidget->setFeature(CDockWidget::DockWidgetPinnable, true);

        _settingsDockWidgetsMap[settingsAction->text()] = settingsDockWidget;

        const auto dockToSettingsActionName = settingsAction->property("DockToDockingActionName").toString();

        CDockAreaWidget* dockAreaWidget = nullptr;

        if (_settingsDockWidgetsMap.contains(dockToSettingsActionName))
            dockAreaWidget = _settingsDockWidgetsMap[dockToSettingsActionName]->dockAreaWidget();

        _dockManager.addDockWidget(static_cast<DockWidgetArea>(settingsAction->property("DockArea").toInt()), settingsDockWidget, dockAreaWidget);

        const auto autoHide         = settingsAction->property("AutoHide").toBool();
        const auto autoHideLocation = static_cast<SideBarLocation>(settingsAction->property("AutoHideLocation").toInt());

        settingsDockWidget->setAutoHide(autoHide, autoHideLocation);

        if (autoHide) {
            switch (settingsDockWidget->autoHideDockContainer()->sideBarLocation())
            {
                case SideBarLocation::SideBarLeft:
                case SideBarLocation::SideBarRight:
                    settingsDockWidget->autoHideDockContainer()->setSize(settingsWidget->minimumSizeHint().width());
                    break;

                case SideBarLocation::SideBarTop:
                case SideBarLocation::SideBarBottom:
                    settingsDockWidget->autoHideDockContainer()->setSize(settingsWidget->minimumSizeHint().height());
                    break;

                default:
                    break;
            }
        }

        const auto studioModeChanged = [settingsDockWidget]() -> void {
            const auto isInStudioMode = projects().getCurrentProject()->getStudioModeAction().isChecked();

            settingsDockWidget->setFeature(CDockWidget::DockWidgetClosable, isInStudioMode);
            settingsDockWidget->setFeature(CDockWidget::DockWidgetMovable, isInStudioMode);
        };

        studioModeChanged();

        connect(&projects().getCurrentProject()->getStudioModeAction(), &ToggleAction::toggled, this, studioModeChanged);

        auto toggleAction = new ToggleAction(this, settingsAction->text(), true);

        _toggleMenu.addAction(toggleAction);

        connect(toggleAction, &ToggleAction::toggled, this, [this, settingsDockWidget](bool toggled) {
            settingsDockWidget->toggleView(toggled);
        });

        connect(settingsDockWidget, &CDockWidget::viewToggled, this, [this, settingsDockWidget, toggleAction, updateHideShowAllActionsReadOnly](bool toggled) {
            QSignalBlocker toggleActionBlocker(toggleAction);
            
            toggleAction->setChecked(toggled);

            updateHideShowAllActionsReadOnly();
        });
    }

    _toggleMenu.setEnabled(!_viewPlugin->getDockingActions().isEmpty());

    if (!_viewPlugin->getDockingActions().isEmpty()) {
        _toggleMenu.addSeparator();
        
        connect(hideAllAction, &TriggerAction::triggered, this, [this]() {
            for (auto settingsDockWidget : _settingsDockWidgetsMap)
                settingsDockWidget->toggleView(false);
        });

        connect(showAllAction, &TriggerAction::triggered, this, [this]() {
            for (auto settingsDockWidget : _settingsDockWidgetsMap)
                settingsDockWidget->toggleView(true);
        });

        _toggleMenu.addAction(hideAllAction);
        _toggleMenu.addAction(showAllAction);
    }

    _dockManager.setStyleSheet("");

    setIcon(viewPlugin->getIcon());
    setWidget(&_dockManager, eInsertMode::ForceNoScrollArea);
    setMinimumSizeHintMode(eMinimumSizeHintMode::MinimumSizeHintFromDockWidget);

    const auto updateWindowTitle = [this]() -> void {
        setWindowTitle(_viewPlugin->getGuiNameAction().getString());
    };

    connect(&viewPlugin->getGuiNameAction(), &StringAction::stringChanged, this, updateWindowTitle);

    updateWindowTitle();

    const auto updateFeatures = [this]() -> void {
        setFeature(CDockWidget::DockWidgetClosable, _viewPlugin->getMayCloseAction().isChecked());
        setFeature(CDockWidget::DockWidgetFloatable, _viewPlugin->getMayFloatAction().isChecked());
        setFeature(CDockWidget::DockWidgetMovable, _viewPlugin->getMayMoveAction().isChecked());
    };

    connect(&viewPlugin->getMayCloseAction(), &ToggleAction::toggled, this, updateFeatures);
    connect(&viewPlugin->getMayFloatAction(), &ToggleAction::toggled, this, updateFeatures);
    connect(&viewPlugin->getMayMoveAction(), &ToggleAction::toggled, this, updateFeatures);

    updateFeatures();

    connect(&viewPlugin->getVisibleAction(), &ToggleAction::toggled, this, [this](bool toggled) {
        toggleView(toggled);
    });

    connect(this, &CDockWidget::viewToggled, this, [this, viewPlugin](bool toggled) {
        QSignalBlocker toggleActionBlocker(&viewPlugin->getVisibleAction());

        viewPlugin->getVisibleAction().setChecked(toggled);
    });
}

ViewPluginDockWidget::SettingsActionWidget::SettingsActionWidget(QWidget* parent, hdps::gui::WidgetAction* settingsAction) :
    QWidget(parent),
    _settingsAction(settingsAction)
{
    Q_ASSERT(settingsAction != nullptr);

    if (settingsAction == nullptr)
        return;

    setAutoFillBackground(true);

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(_settingsAction->createWidget(this));

    setLayout(layout);
}

QSize ViewPluginDockWidget::SettingsActionWidget::minimumSizeHint() const
{
    if (_settingsAction == nullptr)
        return {};

    return _settingsAction->property("MinimumDockWidgetSize").toSize();
}

QSize ViewPluginDockWidget::SettingsActionWidget::sizeHint() const
{
    return minimumSizeHint();
}
