#include "ViewPluginDockWidget.h"

#include <ViewPlugin.h>

#include <util/Serialization.h>

#ifdef _DEBUG
    #define VIEW_PLUGIN_DOCK_WIDGET_VERBOSE
#endif

using namespace ads;

using namespace hdps::plugin;
using namespace hdps::util;

ViewPluginDockWidget::ViewPluginDockWidget(const QString& title, QWidget* parent /*= nullptr*/) :
    DockWidget(title, parent)
{
}

void ViewPluginDockWidget::fromVariantMap(const QVariantMap& variantMap)
{
}

QVariantMap ViewPluginDockWidget::toVariantMap() const
{
    QVariantMap variantMap = DockWidget::toVariantMap();
    
    return variantMap;
}
