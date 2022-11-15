#include "ViewPluginDockWidget.h"

#include <ViewPlugin.h>

#include <util/Serialization.h>

#ifdef _DEBUG
    #define VIEW_PLUGIN_DOCK_WIDGET_VERBOSE
#endif

using namespace ads;

using namespace hdps::plugin;
using namespace hdps::util;

ViewPluginDockWidget::ViewPluginDockWidget(const QString& title, ViewPlugin* viewPlugin, QWidget* parent /*= nullptr*/) :
    DockWidget(title, parent),
    _viewPlugin(viewPlugin)
{
    Q_ASSERT(_viewPlugin != nullptr);

    //setWidget(&_viewPlugin->getWidget());
}

void ViewPluginDockWidget::fromVariantMap(const QVariantMap& variantMap)
{
    _viewPlugin->fromVariantMap(variantMap);
}

QVariantMap ViewPluginDockWidget::toVariantMap() const
{
    QVariantMap variantMap = DockWidget::toVariantMap();

    //variantMap["ViewPlugin"] = const_cast<ViewPluginDockWidget*>(this)->getViewPlugin()->toVariantMap();

    return variantMap;
}

ViewPlugin* ViewPluginDockWidget::getViewPlugin()
{
    return _viewPlugin;
}
