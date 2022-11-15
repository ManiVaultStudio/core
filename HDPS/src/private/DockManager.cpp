#include "DockManager.h"
#include "DockWidget.h"

#include <ViewPlugin.h>

#include <util/Serialization.h>

#include "DockAreaWidget.h"

#include <QSplitter>

#ifdef _DEBUG
    #define DOCK_MANAGER_VERBOSE
#endif

using namespace ads;

using namespace hdps::plugin;
using namespace hdps::util;

QMap<DockWidgetArea, QString> DockManager::dockWidgetAreaStrings = {
    { NoDockWidgetArea, "None" },
    { LeftDockWidgetArea, "Left" },
    { RightDockWidgetArea, "Right" },
    { TopDockWidgetArea, "Top" },
    { BottomDockWidgetArea, "Bottom" },
    { CenterDockWidgetArea, "Center" }
};

DockManager::DockManager(QWidget* parent /*= nullptr*/) :
    CDockManager(parent),
    Serializable("Dock manager"),
    _lastDockAreaWidget(),
    _laggingDockWidgetArea(ads::NoDockWidgetArea)
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
    _rootDockArea = areaFromVariantMap(variantMap["Area"].toMap());

    qDebug() << _rootDockArea;
}

DockManager::DockArea DockManager::areaFromVariantMap(const QVariantMap& areaMap, std::uint32_t depth /*= 0*/)
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << "Adding dock area";
#endif

    //const auto hasSplitter = areaMap["HasSplitter"].toBool();
    //const auto splitterOrientation = static_cast<Qt::Orientation>(areaMap["Orientation"].toInt());

    //if (hasSplitter) {
    //    auto dockWidgetArea = CenterDockWidgetArea;

    //    switch (splitterOrientation)
    //    {
    //    case Qt::Horizontal:
    //        dockWidgetArea = RightDockWidgetArea;
    //        break;

    //    case Qt::Vertical:
    //        dockWidgetArea = BottomDockWidgetArea;
    //        break;

    //    default:
    //        break;
    //    }

    //    _dockWidgetArea = dockWidgetArea;
    //}

    

    DockArea dockArea(depth);

    dockArea.setOrientation(static_cast<Qt::Orientation>(areaMap["Orientation"].toInt()));

    //if (_laggingDockWidgetArea == NoDockWidgetArea)
    //    _laggingDockWidgetArea = _dockWidgetArea;

    if (areaMap.contains("Areas"))
        dockArea.setChildren(areasFromVariantMap(areaMap["Areas"].toList(), depth));

    if (areaMap.contains("DockWidgets"))
        dockArea.setDockWidgets(dockWidgetsFromVariantList(areaMap["DockWidgets"].toList()));

    return dockArea;
}

QVector<DockManager::DockArea> DockManager::areasFromVariantMap(const QVariantList& areasList, std::uint32_t depth)
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << "Adding" << areasList.count() << "dock areas";
#endif

    QVector<DockManager::DockArea> dockAreas;

    for (auto area : areasList)
        dockAreas << areaFromVariantMap(area.toMap(), depth + 1);

    return dockAreas;
}

DockWidgets DockManager::dockWidgetsFromVariantList(const QVariantList& dockWidgetsList)
{
    DockWidgets dockWidgets;

    for (const auto& dockWidget : dockWidgetsList)
        dockWidgets << new DockWidget(dockWidget.toMap()["Title"].toString());

    return dockWidgets;
}

QVariantMap DockManager::toVariantMap() const
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    return {
        { "Area", widgetToVariantMap(rootSplitter()) }
    };
}

QVariantMap DockManager::widgetToVariantMap(QWidget* widget) const
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    QVariantMap widgetMap;

    auto splitter = qobject_cast<QSplitter*>(widget);

    widgetMap["HasSplitter"]    = splitter != nullptr;
    widgetMap["Orientation"]    = 0;
    widgetMap["Sizes"]          = QVariantList();

    if (splitter)
    {
        QVariantList areasList, sizesList;

        for (int i = 0; i < splitter->count(); ++i) {
            areasList.push_back(widgetToVariantMap(splitter->widget(i)));
            sizesList.push_back(splitter->sizes()[i]);
        }
            
        widgetMap["Orientation"]    = static_cast<std::int32_t>(splitter->orientation());
        widgetMap["Sizes"]          = sizesList;
        widgetMap["Areas"]          = areasList;
    }
    else {
        auto dockAreaWidget = qobject_cast<CDockAreaWidget*>(widget);

        if (!dockAreaWidget)
            return widgetMap;

        QVariantList dockWidgetsList;

        for (auto dockWidget : dockAreaWidget->dockWidgets())
            if (dynamic_cast<DockWidget*>(dockWidget))
                dockWidgetsList.push_back(dynamic_cast<DockWidget*>(dockWidget)->toVariantMap());
        
        widgetMap["DockWidgets"] = dockWidgetsList;
    }

    return widgetMap;
}

void DockManager::reset()
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    for (auto dockWidget : dockWidgetsMap().values())
        removeDockWidget(dockWidget);
}
