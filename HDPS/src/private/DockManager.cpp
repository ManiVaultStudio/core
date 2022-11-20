#include "DockManager.h"
#include "ViewPluginDockWidget.h"

#include <ViewPlugin.h>

#include <util/Serialization.h>

#include <DockAreaWidget.h> 

#include <QSplitter>

#ifdef _DEBUG
    #define DOCK_MANAGER_VERBOSE
#endif

using namespace ads;

using namespace hdps;
using namespace hdps::plugin;
using namespace hdps::util;

template <class WidgetClass>
WidgetClass* findParent(const QWidget* widget)
{
    auto parentWidget = widget->parentWidget();

    while (parentWidget)
    {
        auto parentImpl = qobject_cast<WidgetClass*>(parentWidget);

        if (parentImpl)
            return parentImpl;
        
        parentWidget = parentWidget->parentWidget();
    }

    return 0;
}

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
    _centralDockAreaWidget(nullptr),
    _centralDockWidget("CentralDockWidget")
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

ads::CDockAreaWidget* DockManager::findDockAreaWidget(QWidget* widget)
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    for (auto dockWidget : dockWidgets())
        if (dockWidget->widget() == widget)
            return dockWidget->dockAreaWidget();

    return nullptr;
}

ads::CDockAreaWidget* DockManager::setCentralWidget(QWidget* centralWidget)
{
    _centralDockWidget.setWidget(centralWidget);

    _centralDockAreaWidget = CDockManager::setCentralWidget(&_centralDockWidget);

    return _centralDockAreaWidget;
}

ads::CDockAreaWidget* DockManager::getCentralDockAreaWidget()
{
    return _centralDockAreaWidget;
}

DockWidget* DockManager::getCentralDockWidget()
{
    return &_centralDockWidget;
}

void DockManager::fromVariantMap(const QVariantMap& variantMap)
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    reset();

    auto rootDockArea = dockAreaFromVariantMap(variantMap["Area"].toMap());

    rootDockArea.applyDocking();

    //removeDockWidget(rootDockArea.getChildren()[0].getDockWidgets()[0]);
    //setCentralWidget(nullptr);

#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << rootDockArea;
#endif
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

DockArea DockManager::dockAreaFromVariantMap(const QVariantMap& dockAreaMap, std::uint32_t depth /*= 0*/)
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << "Adding dock area";
#endif

    DockArea dockArea(this, depth);

    dockArea.setOrientation(static_cast<Qt::Orientation>(dockAreaMap["Orientation"].toInt()));

    QVector<std::int32_t> splitterSizes;

    for (const auto& splitterSize : dockAreaMap["SplitterSizes"].toList())
        splitterSizes << splitterSize.toInt();

    dockArea.setSplitterSizes(splitterSizes);

    if (dockAreaMap.contains("Areas"))
        dockArea.setChildren(dockAreasFromVariantMap(dockAreaMap["Areas"].toList(), depth));

    if (dockAreaMap.contains("DockWidgets"))
        dockArea.setDockWidgets(dockWidgetsFromVariantList(dockAreaMap["DockWidgets"].toList()));

    return dockArea;
}

QVector<DockArea> DockManager::dockAreasFromVariantMap(const QVariantList& dockAreasList, std::uint32_t depth /*= 0*/)
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << "Adding" << dockAreasList.count() << "dock areas";
#endif

    DockAreas dockAreas;

    for (auto area : dockAreasList)
        dockAreas << dockAreaFromVariantMap(area.toMap(), depth + 1);

    return dockAreas;
}

DockWidgets DockManager::dockWidgetsFromVariantList(const QVariantList& dockWidgetsList)
{
    DockWidgets dockWidgets;

    for (const auto& dockWidget : dockWidgetsList) { 
        const auto dockWidgetMap = dockWidget.toMap();

        if (dockWidgetMap.contains("ViewPlugin"))
            dockWidgets << new ViewPluginDockWidget(dockWidget.toMap()["Title"].toString());
        else {
            if (dockWidget.toMap()["Title"] == "CentralDockWidget")
                dockWidgets << &_centralDockWidget;
            else
                dockWidgets << new DockWidget(dockWidget.toMap()["Title"].toString());
        }

        dockWidgets.last()->fromVariantMap(dockWidgetMap);
    }

    return dockWidgets;
}

QVariantMap DockManager::widgetToVariantMap(QWidget* widget) const
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    QVariantMap widgetMap;

    auto splitter = qobject_cast<QSplitter*>(widget);

    widgetMap["Orientation"]    = 0;
    widgetMap["SplitterSizes"]  = QVariant::fromValue(QVector<std::int32_t>());

    if (splitter)
    {
        QVariantList areasList, sizesList;

        for (int i = 0; i < splitter->count(); ++i) {
            areasList << widgetToVariantMap(splitter->widget(i));
            sizesList << splitter->sizes()[i];
        }
            
        widgetMap["Orientation"]    = static_cast<std::int32_t>(splitter->orientation());
        widgetMap["SplitterSizes"]  = sizesList;
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

    setCentralWidget(&_centralDockWidget);
}
