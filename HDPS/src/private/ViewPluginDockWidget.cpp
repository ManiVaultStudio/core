#include "ViewPluginDockWidget.h"

#include <ViewPlugin.h>

#include <util/Serialization.h>

#ifdef _DEBUG
    #define VIEW_PLUGIN_DOCK_WIDGET_VERBOSE
#endif

using namespace ads;

using namespace hdps;
using namespace hdps::plugin;
using namespace hdps::util;

ViewPluginDockWidget::ViewPluginDockWidget(const QString& title, QWidget* parent /*= nullptr*/) :
    DockWidget(title, parent),
    _viewPlugin(nullptr),
    _viewPluginKind(),
    _viewPluginMap()
{
    getOverlayWidget().show();
}

ViewPluginDockWidget::ViewPluginDockWidget(const QString& title, ViewPlugin* viewPlugin, QWidget* parent /*= nullptr*/) :
    DockWidget(title, parent),
    _viewPlugin(viewPlugin),
    _viewPluginKind(),
    _viewPluginMap()
{
    Q_ASSERT(_viewPlugin != nullptr);

    setWidget(&_viewPlugin->getWidget());
}

void ViewPluginDockWidget::loadViewPlugin()
{
#ifdef VIEW_PLUGIN_DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (Application::core()->isPluginLoaded(_viewPluginKind)) {
        _viewPlugin = Application::core()->requestPlugin<ViewPlugin>(_viewPluginKind);

        if (_viewPlugin) {
            _viewPlugin->fromVariantMap(_viewPluginMap);

            setWidget(&_viewPlugin->getWidget());
        }
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

void ViewPluginDockWidget::fromVariantMap(const QVariantMap& variantMap)
{
    variantMapMustContain(variantMap, "ViewPlugin");

    _viewPluginMap = variantMap["ViewPlugin"].toMap();

    variantMapMustContain(_viewPluginMap, "Plugin");

    const auto pluginMap = _viewPluginMap["Plugin"].toMap();

    variantMapMustContain(pluginMap, "Kind");

    _viewPluginKind = pluginMap["Kind"].toString();
}

QVariantMap ViewPluginDockWidget::toVariantMap() const
{
    QVariantMap variantMap = DockWidget::toVariantMap();

    variantMap["ViewPlugin"] = const_cast<ViewPluginDockWidget*>(this)->getViewPlugin()->toVariantMap();

    return variantMap;
}
