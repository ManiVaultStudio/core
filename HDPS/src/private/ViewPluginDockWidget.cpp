#include "ViewPluginDockWidget.h"

#include <actions/WidgetAction.h>

#include <ViewPlugin.h>

#include <util/Serialization.h>

#ifdef _DEBUG
    //#define VIEW_PLUGIN_DOCK_WIDGET_VERBOSE
#endif

using namespace ads;

using namespace hdps;
using namespace hdps::plugin;
using namespace hdps::util;
using namespace hdps::gui;

ViewPluginDockWidget::ViewPluginDockWidget(const QString& title, QWidget* parent /*= nullptr*/) :
    DockWidget(title, parent),
    _viewPlugin(nullptr),
    _viewPluginKind(),
    _viewPluginMap(),
    _settingsMenu(),
    _helpAction(this, "Help")
{
    setObjectName("ViewPluginDockWidget");

    _helpAction.setIcon(Application::getIconFont("FontAwesome").getIcon("question"));
    _helpAction.setShortcut(tr("F1"));
    _helpAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _helpAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _helpAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);

    connect(&_helpAction, &TriggerAction::triggered, this, [this]() -> void {
        _viewPlugin->getTriggerHelpAction().trigger();
    });

    getOverlayWidget().show();
}

ViewPluginDockWidget::ViewPluginDockWidget(const QString& title, ViewPlugin* viewPlugin, QWidget* parent /*= nullptr*/) :
    DockWidget(title, parent),
    _viewPlugin(nullptr),
    _viewPluginKind(),
    _viewPluginMap(),
    _helpAction(this, "Help")
{
    setViewPlugin(viewPlugin);
    initializeSettingsMenu();
}

QString ViewPluginDockWidget::getTypeString() const
{
    return "ViewPluginDockWidget";
}

void ViewPluginDockWidget::loadViewPlugin()
{
#ifdef VIEW_PLUGIN_DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (Application::core()->isPluginLoaded(_viewPluginKind)) {
        auto viewPlugin = Application::core()->requestPlugin<ViewPlugin>(_viewPluginKind);

        if (viewPlugin)
            setViewPlugin(viewPlugin);
    } else {
        auto& overlayWidget = getOverlayWidget();

        overlayWidget.setColor(QColor(1, 0, 0));
        overlayWidget.set(Application::getIconFont("FontAwesome").getIcon("exclamation-triangle"), "Error loading view", QString("%1 plugin is not loaded").arg(_viewPluginKind));
    }
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

    variantMapMustContain(variantMap, "ViewPlugin");

    _viewPluginMap = variantMap["ViewPlugin"].toMap();

    variantMapMustContain(_viewPluginMap, "Plugin");

    const auto pluginMap = _viewPluginMap["Plugin"].toMap();

    variantMapMustContain(pluginMap, "Kind");

    _viewPluginKind = pluginMap["Kind"].toString();
}

QVariantMap ViewPluginDockWidget::toVariantMap() const
{
#ifdef VIEW_PLUGIN_DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    QVariantMap variantMap = DockWidget::toVariantMap();

    variantMap["ViewPlugin"] = const_cast<ViewPluginDockWidget*>(this)->getViewPlugin()->toVariantMap();

    return variantMap;
}

void ViewPluginDockWidget::initializeSettingsMenu()
{
    if (_viewPlugin->hasHelp())
        _settingsMenu.addAction(&_helpAction);

    _settingsMenu.addAction(&_viewPlugin->getScreenshotAction());
    _settingsMenu.addAction(&_viewPlugin->getIsolateAction());

    _settingsMenu.addSeparator();

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

    _viewPlugin->fromVariantMap(_viewPluginMap);

    setIcon(viewPlugin->getIcon());

    setWidget(&_viewPlugin->getWidget(), eInsertMode::ForceNoScrollArea);

    connect(&viewPlugin->getWidget(), &QWidget::windowTitleChanged, this, [this](const QString& title) {
        setWindowTitle(title);
    });

    connect(&viewPlugin->getMayCloseAction(), &ToggleAction::toggled, this, [this](bool toggled) {
        setFeature(CDockWidget::DockWidgetClosable, toggled);
    });

    connect(&viewPlugin->getMayFloatAction(), &ToggleAction::toggled, this, [this](bool toggled) {
        setFeature(CDockWidget::DockWidgetFloatable, toggled);
    });

    connect(&viewPlugin->getMayMoveAction(), &ToggleAction::toggled, this, [this](bool toggled) {
        setFeature(CDockWidget::DockWidgetMovable, toggled);
    });

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
