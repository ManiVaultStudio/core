#include "DockWidget.h"

#include <ViewPlugin.h>

#ifdef _DEBUG
    #define DOCK_WIDGET_VERBOSE
#endif

using namespace ads;

using namespace hdps::util;
using namespace hdps::plugin;

DockWidget::DockWidget(const QString& title, QWidget* parent /*= nullptr*/) :
    CDockWidget(title, parent),
    Serializable(title)
{
}

void DockWidget::fromVariantMap(const QVariantMap& variantMap)
{

}

QVariantMap DockWidget::toVariantMap() const
{
    QVariantMap variantMap;

    auto viewPluginWidget = dynamic_cast<ViewPlugin*>(widget());

    return {
        { "Title", windowTitle() },
        { "ViewPlugin", viewPluginWidget ? viewPluginWidget->toVariantMap() : QVariantMap() }
    };
}
