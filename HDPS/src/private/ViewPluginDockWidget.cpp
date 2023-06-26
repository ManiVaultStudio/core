#include "ViewPluginDockWidget.h"

#include <Application.h>
#include <CoreInterface.h>

#include <actions/WidgetAction.h>
#include <actions/ToolbarAction.h>

#include <ViewPlugin.h>

#include <util/Serialization.h>

#include <DockWidgetTab.h>
#include <DockAreaWidget.h>
#include <DockAreaTitleBar.h>

#ifdef _DEBUG
    #define VIEW_PLUGIN_DOCK_WIDGET_VERBOSE
#endif

using namespace ads;

using namespace hdps;
using namespace hdps::plugin;
using namespace hdps::util;
using namespace hdps::gui;

QList<ViewPluginDockWidget*> ViewPluginDockWidget::active = QList<ViewPluginDockWidget*>();

ViewPluginDockWidget::ViewPluginDockWidget(const QString& title /*= ""*/, QWidget* parent /*= nullptr*/) :
    DockWidget(title, parent),
    _viewPlugin(nullptr),
    _viewPluginKind(),
    _viewPluginMap(),
    _settingsMenu(),
    _helpAction(this, "Help"),
    _cachedVisibility(false),
    _dockManager(this)
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
    _helpAction(this, "Help"),
    _cachedVisibility(false),
    _dockManager(this)
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
    _helpAction(this, "Help")
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
}

QString ViewPluginDockWidget::getTypeString() const
{
    return "ViewPluginDockWidget";
}

void ViewPluginDockWidget::initialize()
{
    _helpAction.setIcon(Application::getIconFont("FontAwesome").getIcon("question"));
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

    variantMapMustContain(variantMap, "ViewPlugin");

    _viewPluginMap = variantMap["ViewPlugin"].toMap();

    variantMapMustContain(_viewPluginMap, "Kind");

    _viewPluginKind = _viewPluginMap["Kind"].toString();

    loadViewPlugin();
}

QVariantMap ViewPluginDockWidget::toVariantMap() const
{
#ifdef VIEW_PLUGIN_DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    auto variantMap = DockWidget::toVariantMap();

    if (_viewPlugin)
        variantMap["ViewPlugin"] = const_cast<ViewPluginDockWidget*>(this)->getViewPlugin()->toVariantMap();

    //variantMap["DockManager"]
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

    centralDockWidget->setWidget(&_viewPlugin->getWidget());

    _dockManager.setCentralWidget(centralDockWidget);
    
    for (auto settingsAction : _viewPlugin->getSettingsActions()) {
        auto settingsDockWidget = new CDockWidget(settingsAction->text());
        auto settingsWidget     = settingsAction->createWidget(settingsDockWidget);

        settingsWidget->setAutoFillBackground(true);

        settingsDockWidget->setWidget(settingsWidget, eInsertMode::ForceNoScrollArea);
        settingsDockWidget->setMinimumSizeHintMode(eMinimumSizeHintMode::MinimumSizeHintFromDockWidget);
        settingsDockWidget->setAutoFillBackground(true);

        _dockManager.addDockWidget(DockWidgetArea::AllDockAreas, settingsDockWidget);

        auto toolbarAction = dynamic_cast<ToolbarAction*>(settingsAction);

        if (toolbarAction) {
            const auto studioModeChanged = [settingsDockWidget]() -> void {
                const auto isInStudioMode = projects().getCurrentProject()->getStudioModeAction().isChecked();

                settingsDockWidget->tabWidget()->setVisible(isInStudioMode);
                settingsDockWidget->dockAreaWidget()->titleBar()->setVisible(isInStudioMode);

                settingsDockWidget->setFeature(CDockWidget::DockWidgetClosable, isInStudioMode);
                settingsDockWidget->setFeature(CDockWidget::DockWidgetMovable, isInStudioMode);
                settingsDockWidget->setFeature(CDockWidget::DockWidgetFloatable, isInStudioMode);
            };

            studioModeChanged();

            connect(&projects().getCurrentProject()->getStudioModeAction(), &ToggleAction::toggled, this, studioModeChanged);
        }
        else {
            settingsDockWidget->setFeature(CDockWidget::DockWidgetPinnable, true);
        }
    }

    setIcon(viewPlugin->getIcon());
    //setWidget(&_viewPlugin->getWidget(), eInsertMode::ForceNoScrollArea);
    setWidget(&_dockManager);
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

    const auto connectToViewPluginVisibleAction = [this, viewPlugin](CDockWidget* dockWidget) -> void {
        connect(&viewPlugin->getVisibleAction(), &ToggleAction::toggled, this, [this, dockWidget](bool toggled) {
            dockWidget->toggleView(toggled);
        });
    };

    const auto disconnectFromViewPluginVisibleAction = [this, viewPlugin](CDockWidget* dockWidget) -> void {
        disconnect(&viewPlugin->getVisibleAction(), &ToggleAction::toggled, this, nullptr);
    };

    connect(this, &CDockWidget::viewToggled, this, [this, viewPlugin, connectToViewPluginVisibleAction, disconnectFromViewPluginVisibleAction](bool toggled) {
        disconnectFromViewPluginVisibleAction(this);
        {
            viewPlugin->getVisibleAction().setChecked(toggled);
        }
        connectToViewPluginVisibleAction(this);
    });

    connectToViewPluginVisibleAction(this);
}
