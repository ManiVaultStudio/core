#include "DockManager.h"
#include "DockWidget.h"

#include <ViewPlugin.h>

#include <util/Serialization.h>

#include "DockAreaWidget.h"

#include <QSplitter>

//#ifdef _DEBUG
//    #define DOCK_MANAGER_VERBOSE
//#endif

using namespace ads;

using namespace hdps::plugin;
using namespace hdps::util;

DockManager::DockManager(QWidget* parent /*= nullptr*/) :
    CDockManager(parent),
    Serializable("Dock manager")
{
    CDockManager::setConfigFlag(CDockManager::DragPreviewIsDynamic, true);
    CDockManager::setConfigFlag(CDockManager::DragPreviewShowsContentPixmap, true);
    CDockManager::setConfigFlag(CDockManager::DragPreviewShowsContentPixmap, true);
    CDockManager::setConfigFlag(CDockManager::DragPreviewHasWindowFrame, true);
    CDockManager::setConfigFlag(CDockManager::OpaqueSplitterResize, false);
    CDockManager::setConfigFlag(CDockManager::FocusHighlighting, true);
    CDockManager::setConfigFlag(CDockManager::EqualSplitOnInsertion, true);
    CDockManager::setConfigFlag(CDockManager::DockAreaHasUndockButton, true);
    CDockManager::setConfigFlag(CDockManager::DockAreaHasCloseButton, false);
    CDockManager::setConfigFlag(CDockManager::DockAreaHasUndockButton, false);
    CDockManager::setConfigFlag(CDockManager::DockAreaHasTabsMenuButton, true);
    CDockManager::setConfigFlag(CDockManager::DockAreaDynamicTabsMenuButtonVisibility, false);
}

void DockManager::fromVariantMap(const QVariantMap& variantMap)
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    reset();
    loadDockAreasFromVariantMap(variantMap);
}

QVariantMap DockManager::toVariantMap() const
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    return toVariantMap(rootSplitter());
}

void DockManager::reset()
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    for (auto dockWidget : dockWidgetsMap().values()) {
        auto widget = dockWidget->takeWidget();

        if (widget)
            delete widget;

        removeDockWidget(dockWidget);
    }
}

void DockManager::loadDockAreasFromVariantMap(const QVariantMap& variantMap) const
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (!variantMap.contains("DockAreas"))
        return;

    for (const auto& dockAreaMap : variantMap["DockAreas"].toList())
        loadDockAreaFromVariantMap(dockAreaMap.toMap());
}

void DockManager::loadDockAreaFromVariantMap(const QVariantMap& variantMap) const
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    qDebug() << variantMap;
}

QVariantMap DockManager::toVariantMap(QWidget* widget) const
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    QVariantMap variantMap;

    auto splitter = qobject_cast<QSplitter*>(widget);

    if (splitter)
    {
        QVariantList widgetsList;
        QVariantList sizesList;

        for (int i = 0; i < splitter->count(); ++i) {
            widgetsList.push_back(toVariantMap(splitter->widget(i)));
            sizesList.push_back(splitter->sizes()[i]);
        }
            
        variantMap["Splitter"] = QVariantMap({
            { "Enabled", splitter->count() == 2 },
            { "Orientation", static_cast<std::int32_t>(splitter->orientation()) },
            { "Size", sizesList }
        });

        variantMap["DockAreas"] = widgetsList;
    }
    else {
        auto dockAreaWidget = qobject_cast<CDockAreaWidget*>(widget);

        if (!dockAreaWidget)
            return variantMap;

        QVariantList dockWidgetsList;

        for (auto dockWidget : dockAreaWidget->dockWidgets())
            if (dynamic_cast<DockWidget*>(dockWidget))
                dockWidgetsList.push_back(dynamic_cast<DockWidget*>(dockWidget)->toVariantMap());

            //QVariantMap dockWidgetMap;

            //dockWidgetMap["Title"] = dockWidget->windowTitle();

            //auto viewPluginWidget = dynamic_cast<ViewPlugin*>(dockWidget->widget());

            //if (viewPluginWidget)
            //    dockWidgetMap["ViewPlugin"] = viewPluginWidget->toVariantMap();
            //
            //dockWidgetsList.push_back(dockWidgetMap);
        //}
        
        variantMap["DockWidgets"] = dockWidgetsList;
    }

    return variantMap;
}
