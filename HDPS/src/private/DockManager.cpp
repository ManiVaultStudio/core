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

DockManager::DockArea::DockArea(DockManager* dockManager, std::uint32_t depth /*= 0*/) :
    _dockManager(dockManager),
    _parent(nullptr),
    _depth(depth),
    _orientation(),
    _children(),
    _dockWidgets(),
    _lastDockAreaWidget(nullptr)
{
}

DockManager::DockArea::DockArea(const DockArea& other)
{
    *this = other;
}

DockManager::DockArea* DockManager::DockArea::getParent()
{
    return _parent;
}

void DockManager::DockArea::setParent(DockArea* parent)
{
    _parent = parent;
}

std::uint32_t DockManager::DockArea::getDepth() const
{
    return _depth;
}

void DockManager::DockArea::setDepth(std::uint32_t depth)
{
    _depth = depth;
}

Qt::Orientation DockManager::DockArea::getOrientation() const
{
    return _orientation;
}

void DockManager::DockArea::setOrientation(Qt::Orientation orientation)
{
    _orientation = orientation;
}

DockManager::DockAreas DockManager::DockArea::getChildren() const
{
    return _children;
}

void DockManager::DockArea::setChildren(DockAreas children)
{
    _children = children;

    for (auto& child : _children) {
        child.setParent(this);
        child.setDepth(_depth + 1);
    }
}

DockWidgets DockManager::DockArea::getDockWidgets() const
{
    return _dockWidgets;
}

void DockManager::DockArea::setDockWidgets(DockWidgets dockWidgets)
{
    _dockWidgets = dockWidgets;
}

ads::CDockAreaWidget* DockManager::DockArea::getLastDockAreaWidget()
{
    return _lastDockAreaWidget;
}

void DockManager::DockArea::setLastDockAreaWidget(ads::CDockAreaWidget* lastDockAreaWidget)
{
    _lastDockAreaWidget = lastDockAreaWidget;
}

bool DockManager::DockArea::hasLastDockAreaWidget() const
{
    return _lastDockAreaWidget != nullptr;
}

void DockManager::DockArea::createPlaceHolders(int depth)
{
    qDebug() << __FUNCTION__;

    if (_depth < depth) {
        for (auto& child : _children)
            child.createPlaceHolders(depth);
    }

    if (_depth == depth) {
        auto dockWidgetArea = CenterDockWidgetArea;

        switch (getParent()->getOrientation())
        {
            case Qt::Horizontal:
                dockWidgetArea = RightDockWidgetArea;
                break;

            case Qt::Vertical:
                dockWidgetArea = BottomDockWidgetArea;
                break;

            default:
                break;
        }

        dockWidgetArea = getParent()->hasLastDockAreaWidget() ? dockWidgetArea : CenterDockWidgetArea;

        auto dockWidget             = new DockWidget("Dock widget");
        auto parentLastDockWidget   = getParent()->getLastDockAreaWidget();

        if (parentLastDockWidget)
            getParent()->setLastDockAreaWidget(_dockManager->addDockWidget(dockWidgetArea, dockWidget, parentLastDockWidget));
        else
            getParent()->setLastDockAreaWidget(_dockManager->addDockWidget(dockWidgetArea, dockWidget));

        setLastDockAreaWidget(getParent()->getLastDockAreaWidget());
    }
}

void DockManager::DockArea::applyDocking(int depth)
{
    //if (_depth < depth) {
    //    for (auto child : _children)
    //        child.applyDocking(depth);
    //}

    //if (_depth == depth) {
    //    if (getParent() == nullptr)
    //        return;

    //    auto dockWidgetArea = CenterDockWidgetArea;

    //    switch (getParent()->getOrientation())
    //    {
    //        case Qt::Horizontal:
    //            dockWidgetArea = RightDockWidgetArea;
    //            break;

    //        case Qt::Vertical:
    //            dockWidgetArea = BottomDockWidgetArea;
    //            break;

    //        default:
    //            break;
    //    }
    //    
    //    dockWidgetArea = getParent()->hasLastDockAreaWidget() ? dockWidgetArea : CenterDockWidgetArea;

    //    auto dockWidget = new DockWidget("Dock widget");
    //    auto parentLastDockWidget = getParent()->getLastDockAreaWidget();

    //    if (parentLastDockWidget)
    //        getParent()->setLastDockAreaWidget(_dockManager->addDockWidget(dockWidgetArea, dockWidget, parentLastDockWidget));
    //    else
    //        getParent()->setLastDockAreaWidget(_dockManager->addDockWidget(dockWidgetArea, dockWidget));
    //}
}

void DockManager::DockArea::applyDocking()
{
    sanitizeHierarchy();
    createPlaceHolders(1);
    createPlaceHolders(2);
    createPlaceHolders(3);
    /*
    if (!_children.isEmpty()) {
        for (auto child : _children)
            child.applyDocking();
    }
    else {
        
    }

    
    if (_depth < depth) {
        for (auto child : _children)
            child.applyDocking(depth);
    }
    else if (_depth == depth) {
        

        _lastDockAreaWidget = _dockManager->addDockWidget(dockWidgetArea, new DockWidget("Dock widget"), _lastDockAreaWidget);
    }
    */
}

void DockManager::DockArea::sanitizeHierarchy()
{
    for (auto& child : _children) {
        child.setParent(this);
        child.sanitizeHierarchy();
    }
}

DockWidget* DockManager::DockArea::getFirstDockWidget()
{
    if (_dockWidgets.count() >= 1)
        return _dockWidgets.first();

    for (auto child : _children) {
        auto firstDockWidget = child.getFirstDockWidget();

        if (firstDockWidget)
            return firstDockWidget;
    }

    return nullptr;
}

QMap<DockWidgetArea, QString> DockManager::dockWidgetAreaStrings = {
    { NoDockWidgetArea, "None" },
    { LeftDockWidgetArea, "Left" },
    { RightDockWidgetArea, "Right" },
    { TopDockWidgetArea, "Top" },
    { BottomDockWidgetArea, "Bottom" },
    { CenterDockWidgetArea, "Center" }
};

QMap<Qt::Orientation, QString> DockManager::orientationStrings = {
    { Qt::Horizontal, "Horizontal" },
    { Qt::Vertical, "Vertical" }
};

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

    auto rootDockArea = areaFromVariantMap(variantMap["Area"].toMap());

    rootDockArea.applyDocking();
    //rootDockArea.applyDocking(2);

#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << rootDockArea;
#endif
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

    

    DockArea dockArea(this, depth);

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
