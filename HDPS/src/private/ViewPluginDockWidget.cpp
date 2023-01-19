#include "ViewPluginDockWidget.h"

#include <Application.h>
#include <CoreInterface.h>

#include <actions/WidgetAction.h>

#include <ViewPlugin.h>

#include <util/Serialization.h>

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
    _helpAction(this, "Help")
{
    active << this;

    setFeature(CDockWidget::DockWidgetDeleteOnClose, false);

    _helpAction.setIcon(Application::getIconFont("FontAwesome").getIcon("question"));
    _helpAction.setShortcut(tr("F1"));
    _helpAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _helpAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _helpAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);

    connect(&_helpAction, &TriggerAction::triggered, this, [this]() -> void {
        _viewPlugin->getTriggerHelpAction().trigger();
    });

    initialize();
}

ViewPluginDockWidget::ViewPluginDockWidget(const QString& title, ViewPlugin* viewPlugin, QWidget* parent /*= nullptr*/) :
    DockWidget(title, parent),
    _viewPlugin(nullptr),
    _viewPluginKind(),
    _viewPluginMap(),
    _helpAction(this, "Help")
{
    active << this;

    setFeature(CDockWidget::DockWidgetDeleteOnClose, false);
    initialize();
    setViewPlugin(viewPlugin);
    initializeSettingsMenu();
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
    connect(&plugins(), &AbstractPluginManager::pluginAboutToBeDestroyed, this, [this](plugin::Plugin* plugin) -> void {
        if (plugin != _viewPlugin)
            return;

#ifdef VIEW_PLUGIN_DOCK_WIDGET_VERBOSE
        qDebug() << "Remove active view plugin dock widget" << plugin->getGuiName();
#endif

        active.removeOne(this);

        takeWidget();
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
    
    if (_viewPlugin)
        _viewPlugin->fromVariantMap(_viewPluginMap);
}

QVariantMap ViewPluginDockWidget::toVariantMap() const
{
#ifdef VIEW_PLUGIN_DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    auto variantMap = DockWidget::toVariantMap();

    if (_viewPlugin)
        variantMap["ViewPlugin"] = const_cast<ViewPluginDockWidget*>(this)->getViewPlugin()->toVariantMap();

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

void ViewPluginDockWidget::initializeSettingsMenu()
{
    if (_viewPlugin->hasHelp())
        _settingsMenu.addAction(&_helpAction);

    _settingsMenu.addAction(&_viewPlugin->getScreenshotAction());

    if (!_viewPlugin->isSystemViewPlugin())
        _settingsMenu.addAction(&_viewPlugin->getIsolateAction());

    _settingsMenu.addSeparator();

#ifdef _DEBUG
    _settingsMenu.addAction(&_viewPlugin->getDestroyAction());
#endif
    
    _settingsMenu.addAction(&_viewPlugin->getEditActionsAction());
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

    setIcon(viewPlugin->getIcon());
    setWidget(&_viewPlugin->getWidget(), eInsertMode::ForceNoScrollArea);

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

    QObject::connect(this, &CDockWidget::viewToggled, this, [this, viewPlugin, connectToViewPluginVisibleAction, disconnectFromViewPluginVisibleAction](bool toggled) {
        disconnectFromViewPluginVisibleAction(this);
        {
            viewPlugin->getVisibleAction().setChecked(toggled);
        }
        connectToViewPluginVisibleAction(this);
    });

    connectToViewPluginVisibleAction(this);

    initializeSettingsMenu();
}
