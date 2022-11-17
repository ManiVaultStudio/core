#include "DockArea.h"
#include "DockManager.h"
#include "ViewPluginDockWidget.h"

#include <DockAreaWidget.h>

#include <QSplitter>

#ifdef _DEBUG
    #define DOCK_AREA_VERBOSE
#endif

using namespace ads;

using namespace hdps;
using namespace hdps::plugin;
using namespace hdps::util;

QMap<Qt::Orientation, QString> DockArea::orientationStrings = {
    { Qt::Horizontal, "Horizontal" },
    { Qt::Vertical, "Vertical" }
};

DockArea::DockArea(DockManager* dockManager, std::uint32_t depth /*= 0*/) :
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

DockArea::DockArea(const DockArea& other)
{
    *this = other;
}

QString DockArea::getId() const
{
    return _id;
}

DockArea* DockArea::getParent()
{
    return _parent;
}

void DockArea::setParent(DockArea* parent)
{
    _parent = parent;
}

std::uint32_t DockArea::getDepth() const
{
    return _depth;
}

void DockArea::setDepth(std::uint32_t depth)
{
    _depth = depth;
}

DockArea::SplitterSizes DockArea::getSplitterSizes() const
{
    return _splitterSizes;
}

void DockArea::setSplitterSizes(const SplitterSizes& splitterSizes)
{
    _splitterSizes = splitterSizes;
}

Qt::Orientation DockArea::getOrientation() const
{
    return _orientation;
}

void DockArea::setOrientation(Qt::Orientation orientation)
{
    _orientation = orientation;
}

DockAreas DockArea::getChildren() const
{
    return _children;
}

void DockArea::setChildren(DockAreas children)
{
    _children = children;

    for (auto& child : _children) {
        child.setParent(this);
        child.setDepth(_depth + 1);
    }
}

std::uint32_t DockArea::getChildIndex(const DockArea& child) const
{
    return _children.indexOf(child);
}

DockWidgets DockArea::getDockWidgets() const
{
    return _dockWidgets;
}

void DockArea::setDockWidgets(DockWidgets dockWidgets)
{
    _dockWidgets = dockWidgets;
}

ads::CDockAreaWidget* DockArea::getCurrentDockAreaWidget()
{
    return _currentDockAreaWidget;
}

void DockArea::setCurrentDockAreaWidget(ads::CDockAreaWidget* lastDockAreaWidget)
{
    _currentDockAreaWidget = lastDockAreaWidget;
}

bool DockArea::hasLastDockAreaWidget() const
{
    return _currentDockAreaWidget != nullptr;
}

void DockArea::createPlaceHolders(std::uint32_t depth)
{
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

void DockArea::removePlaceHolders()
{
    for (auto& child : _children)
        child.removePlaceHolders();

    if (_placeholderDockWidget)
        _dockManager->removeDockWidget(_placeholderDockWidget);

    delete _placeholderDockWidget;
}

void DockArea::applyDocking()
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

void DockArea::sanitizeHierarchy()
{
    for (auto& child : _children) {
        child.setParent(this);
        child.sanitizeHierarchy();
    }
}

DockWidget* DockArea::getFirstDockWidget()
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

void DockArea::loadViewPluginDockWidgets()
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

std::uint32_t DockArea::getMaxDepth() const
{
    std::uint32_t maxDepth = _depth;

    for (auto& child : _children) {
        const auto childMaxDepth = child.getMaxDepth();

        if (childMaxDepth > maxDepth)
            maxDepth = childMaxDepth;
    }
        
    return maxDepth;
}

void DockArea::setSplitterSizes()
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
