#include "DockManager.h"
#include "DockWidget.h"
#include "ViewPluginDockWidget.h"

#include <ViewPlugin.h>

#include <util/Serialization.h>

#include "DockAreaWidget.h"

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
    QWidget* parentWidget = widget->parentWidget();

    while (parentWidget)
    {
        auto parentImpl = qobject_cast<WidgetClass*>(parentWidget);

        if (parentImpl)
            return parentImpl;
        
        parentWidget = parentWidget->parentWidget();
    }

    return 0;
}

DockManager::DockArea::DockArea(DockManager* dockManager, std::uint32_t depth /*= 0*/) :
    _dockManager(dockManager),
    _parent(nullptr),
    _depth(depth),
    _splitterSizes(),
    _orientation(),
    _children(),
    _placeholderDockWidget(nullptr),
    _dockWidgets(),
    _currentDockAreaWidget(nullptr)
{
}

DockManager::DockArea::DockArea(const DockArea& other)
{
    *this = other;
}

QString DockManager::DockArea::getId() const
{
    return _id;
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

DockManager::DockArea::SplitterSizes DockManager::DockArea::getSplitterSizes() const
{
    return _splitterSizes;
}

void DockManager::DockArea::setSplitterSizes(const SplitterSizes& splitterSizes)
{
    _splitterSizes = splitterSizes;
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

std::uint32_t DockManager::DockArea::getChildIndex(const DockArea& child) const
{
    return _children.indexOf(child);
}

DockWidgets DockManager::DockArea::getDockWidgets() const
{
    return _dockWidgets;
}

void DockManager::DockArea::setDockWidgets(DockWidgets dockWidgets)
{
    _dockWidgets = dockWidgets;
}

ads::CDockAreaWidget* DockManager::DockArea::getCurrentDockAreaWidget()
{
    return _currentDockAreaWidget;
}

void DockManager::DockArea::setCurrentDockAreaWidget(ads::CDockAreaWidget* lastDockAreaWidget)
{
    _currentDockAreaWidget = lastDockAreaWidget;
}

bool DockManager::DockArea::hasLastDockAreaWidget() const
{
    return _currentDockAreaWidget != nullptr;
}

void DockManager::DockArea::createPlaceHolders(std::uint32_t depth)
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

        auto dockWidget             = _dockWidgets.count() == 0 ? new DockWidget("Placeholder dock widget") : _dockWidgets.first();
        auto targetDockWidgetArea   = getParent()->getCurrentDockAreaWidget();

        if (dockWidgetArea)
            getParent()->setCurrentDockAreaWidget(_dockManager->addDockWidget(dockWidgetArea, dockWidget, targetDockWidgetArea));
        else
            getParent()->setCurrentDockAreaWidget(_dockManager->addDockWidget(dockWidgetArea, dockWidget));

        setCurrentDockAreaWidget(getParent()->getCurrentDockAreaWidget());

        if (_dockWidgets.isEmpty())
            _placeholderDockWidget = dockWidget;

        if (_dockWidgets.count() >= 2) {
            for (auto dockWidget : _dockWidgets) {
                if (dockWidget == _dockWidgets.first())
                    continue;

                setCurrentDockAreaWidget(_dockManager->addDockWidget(CenterDockWidgetArea, dockWidget, getCurrentDockAreaWidget()));
            }
        }
    }
}

void DockManager::DockArea::removePlaceHolders()
{
    for (auto& child : _children)
        child.removePlaceHolders();

    if (_placeholderDockWidget)
        _dockManager->removeDockWidget(_placeholderDockWidget);

    delete _placeholderDockWidget;
}

void DockManager::DockArea::applyDocking()
{
    sanitizeHierarchy();

    for (std::uint32_t depth = 1; depth <= getMaxDepth(); depth++)
        createPlaceHolders(depth);
    
    Application::processEvents();

    removePlaceHolders();

    Application::processEvents();

    setSplitterSizes();
    loadViewPluginDockWidgets();
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

void DockManager::DockArea::loadViewPluginDockWidgets()
{
    for (auto& child : _children)
        child.loadViewPluginDockWidgets();

    for (auto& dockWidget : _dockWidgets) {
        auto viewPluginDockWidget = dynamic_cast<ViewPluginDockWidget*>(dockWidget);

        if (!viewPluginDockWidget)
            continue;

        viewPluginDockWidget->loadViewPlugin();
    }
}

std::uint32_t DockManager::DockArea::getMaxDepth() const
{
    std::uint32_t maxDepth = _depth;

    for (auto& child : _children) {
        const auto childMaxDepth = child.getMaxDepth();

        if (childMaxDepth > maxDepth)
            maxDepth = childMaxDepth;
    }
        
    return maxDepth;
}

void DockManager::DockArea::setSplitterSizes()
{
    if (_currentDockAreaWidget) {
        //if (_depth == 0)
        //    _dockManager->rootSplitter()->setSizes(getSplitterSizes());
        //else
        //    _dockManager->setSplitterSizes(_currentDockAreaWidget, getSplitterSizes());
        /*auto parentSplitter = findParent<QSplitter>(_currentDockAreaWidget);

        if (parentSplitter)
            
            parentSplitter->setSizes(getSplitterSizes());*/
    }

    for (auto child : _children)
        child.setSplitterSizes();
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

#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << rootDockArea;
#endif
}

DockManager::DockArea DockManager::areaFromVariantMap(const QVariantMap& areaMap, std::uint32_t depth /*= 0*/)
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << "Adding dock area";
#endif

    DockArea dockArea(this, depth);

    dockArea.setOrientation(static_cast<Qt::Orientation>(areaMap["Orientation"].toInt()));

    QVector<std::int32_t> splitterSizes;

    for (const auto& splitterSize : areaMap["SplitterSizes"].toList())
        splitterSizes << splitterSize.toInt();

    dockArea.setSplitterSizes(splitterSizes);

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

    for (const auto& dockWidget : dockWidgetsList) { 
        const auto dockWidgetMap = dockWidget.toMap();

        if (dockWidgetMap.contains("ViewPlugin"))
            dockWidgets << new ViewPluginDockWidget(dockWidget.toMap()["Title"].toString());
        else
            dockWidgets << new DockWidget(dockWidget.toMap()["Title"].toString());

        dockWidgets.last()->fromVariantMap(dockWidgetMap);
    }

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
}
