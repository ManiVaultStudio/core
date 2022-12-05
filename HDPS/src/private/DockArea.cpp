#include "DockArea.h"
#include "DockManager.h"
#include "ViewPluginDockWidget.h"

#include <util/Miscellaneous.h>
#include <util/Serialization.h>

#include <DockAreaWidget.h>

#include <QSplitter>
#include <QUuid>

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
    _id(QUuid::createUuid().toString()),
    _dockManager(dockManager),
    _parent(nullptr),
    _depth(depth),
    _splitterRatios(),
    _orientation(),
    _children(),
    _isCentral(false),
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

bool DockArea::hasParent() const
{
    return _parent != nullptr;
}

DockArea* DockArea::getParent()
{
    return _parent;
}

void DockArea::setParent(DockArea* parent)
{
    _parent = parent;
}

bool DockArea::isRoot() const
{
    return _depth == 0;
}

std::uint32_t DockArea::getDepth() const
{
    return _depth;
}

void DockArea::setDepth(std::uint32_t depth)
{
    _depth = depth;
}

DockArea::SplitterRatios DockArea::getSplitterRatios() const
{
    return _splitterRatios;
}

void DockArea::setSplitterRatios(const SplitterRatios& splitterRatios)
{
    _splitterRatios = splitterRatios;
}

void DockArea::fromVariantMap(const QVariantMap& variantMap)
{
    variantMapMustContain(variantMap, "IsCentral");

    setCentral(variantMap["IsCentral"].toBool());

    if (variantMap.contains("Children")) {

        variantMapMustContain(variantMap, "Orientation");
        variantMapMustContain(variantMap, "SplitterRatios");

        setOrientation(static_cast<Qt::Orientation>(variantMap["Orientation"].toInt()));
        
        SplitterRatios splitterRatios;

        for (const auto& splitterRatio : variantMap["SplitterRatios"].toList())
            splitterRatios << splitterRatio.toFloat();

        setSplitterRatios(splitterRatios);

        DockAreas children;

        for (auto child : variantMap["Children"].toList()) {
            DockArea childDockArea(_dockManager, _depth + 1);

            childDockArea.fromVariantMap(child.toMap());

            children << childDockArea;
        }
        
        setChildren(children);
    }
    else {
        variantMapMustContain(variantMap, "DockWidgets");

        DockWidgets dockWidgets;

        const QStringList ignoreTypes{ "CentralDockWidget", "ViewPluginsDockWidget" };

        for (const auto& dockWidget : variantMap["DockWidgets"].toList()) {
            const auto dockWidgetMap    = dockWidget.toMap();
            const auto dockWidgetType   = dockWidgetMap["Type"].toString();

            if (dockWidgetType == "ViewPluginDockWidget")
                _dockWidgets << new ViewPluginDockWidget(dockWidget.toMap()["Title"].toString());
            else {
                if (!ignoreTypes.contains(dockWidgetType))
                    _dockWidgets << new DockWidget(dockWidget.toMap()["Title"].toString());
            }

            if (!_dockWidgets.isEmpty())
                _dockWidgets.last()->fromVariantMap(dockWidgetMap);
        }
    }
}

QVariantMap DockArea::toVariantMap() const
{
    QVariantMap variantMap;

    if (!_children.isEmpty()) {
        QVariantList childrenList, splitterRatiosList;

        const auto splitterRatios = getSplitterRatios();

        for (auto& child : _children) {
            childrenList << child.toVariantMap();

            auto currentDockAreaWidget = const_cast<DockArea&>(child).getCurrentDockAreaWidget();

            if (currentDockAreaWidget && !currentDockAreaWidget->isVisible())
                continue;

            splitterRatiosList << splitterRatios[_children.indexOf(child)];
        }

        variantMap["Orientation"]       = static_cast<std::int32_t>(getOrientation());
        variantMap["SplitterRatios"]    = splitterRatiosList;
        variantMap["Children"]          = childrenList;
    }
    else {
        QVariantList dockWidgetsList;

        for (auto dockWidget : _dockWidgets)
            if (dynamic_cast<DockWidget*>(dockWidget))
                dockWidgetsList.push_back(dynamic_cast<DockWidget*>(dockWidget)->toVariantMap());

        variantMap["DockWidgets"] = dockWidgetsList;
    }

    variantMap["IsCentral"] = QVariant::fromValue(isCentral());

    return variantMap;
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

    for (auto& child : _children)
        child.setParent(this);
}

bool DockArea::isCentral() const
{
    return _isCentral;
}

void DockArea::setCentral(bool central)
{
    _isCentral = central;
}

std::uint32_t DockArea::getChildIndex(const DockArea& child) const
{
    return _children.indexOf(child);
}

bool DockArea::hasDockWidgets() const
{
    return !_dockWidgets.isEmpty();
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

bool DockArea::hasCurrentDockAreaWidget() const
{
    return _currentDockAreaWidget != nullptr;
}

void DockArea::createDockWidgets(std::uint32_t depth)
{
    const auto childIndex = hasParent() ? getParent()->getChildIndex(*this) : -1;

    if (_depth < depth) {
        for (auto& child : _children)
            child.createDockWidgets(depth);
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

        dockWidgetArea = getParent()->hasCurrentDockAreaWidget() ? dockWidgetArea : CenterDockWidgetArea;

        auto dockWidget             = _dockWidgets.count() == 0 ? new DockWidget("Placeholder dock widget") : _dockWidgets.first();
        auto targetDockWidgetArea   = getParent()->getCurrentDockAreaWidget();
        
        if (_dockManager->objectName() == "MainDockManager") {
            std::int32_t centralWidgetIndex = -1;

            if (hasParent()) {
                for (auto child : getParent()->getChildren())
                    if (child.isCentral())
                        centralWidgetIndex = getParent()->getChildIndex(child);
            }

            if (centralWidgetIndex >= 0) {
                if (childIndex == 0) {
                    switch (getParent()->getOrientation())
                    {
                        case Qt::Horizontal:
                            dockWidgetArea = LeftDockWidgetArea;
                            break;

                        case Qt::Vertical:
                            dockWidgetArea = TopDockWidgetArea;
                            break;
                    }

                    getParent()->setCurrentDockAreaWidget(_dockManager->addDockWidget(dockWidgetArea, dockWidget, _dockManager->getCentralDockAreaWidget()));
                }

                if (childIndex > 0 && childIndex < static_cast<std::uint32_t>(centralWidgetIndex)) {
                    getParent()->setCurrentDockAreaWidget(_dockManager->addDockWidget(dockWidgetArea, dockWidget, getParent()->getCurrentDockAreaWidget()));
                }

                if (childIndex == static_cast<std::uint32_t>(centralWidgetIndex)) {
                    getParent()->setCurrentDockAreaWidget(_dockManager->addDockWidget(dockWidgetArea, dockWidget, _dockManager->getCentralDockAreaWidget()));
                }

                if (childIndex > static_cast<std::uint32_t>(centralWidgetIndex)) {
                    getParent()->setCurrentDockAreaWidget(_dockManager->addDockWidget(dockWidgetArea, dockWidget, getParent()->getCurrentDockAreaWidget()));
                }
            }
            else {
                getParent()->setCurrentDockAreaWidget(_dockManager->addDockWidget(dockWidgetArea, dockWidget, getParent()->getCurrentDockAreaWidget()));
            }
        }
        
        if (_dockManager->objectName() == "ViewPluginsDockManager") {
            if (targetDockWidgetArea)
                getParent()->setCurrentDockAreaWidget(_dockManager->addDockWidget(dockWidgetArea, dockWidget, targetDockWidgetArea));
            else
                getParent()->setCurrentDockAreaWidget(_dockManager->addDockWidget(dockWidgetArea, dockWidget));
        }

        setCurrentDockAreaWidget(getParent()->getCurrentDockAreaWidget());

        auto parentSplitter = dynamic_cast<QSplitter*>(getCurrentDockAreaWidget()->parentWidget());

        if (parentSplitter) {
            QVariantList splitterRatios;

            for (const auto& splitterRatio : getParent()->getSplitterRatios())
                splitterRatios << splitterRatio;

            getCurrentDockAreaWidget()->setProperty("SplitterRatios", splitterRatios);

            parentSplitter->setProperty("SplitterRatios", splitterRatios);
        }

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

void DockArea::removePlaceHolderDockWidgets()
{
    for (auto& child : _children)
        child.removePlaceHolderDockWidgets();

    if (_placeholderDockWidget)
        _dockManager->removeDockWidget(_placeholderDockWidget);

    delete _placeholderDockWidget;
}

void DockArea::applyDocking()
{
#ifdef DOCK_AREA_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    sanitizeHierarchy();

    for (std::uint32_t depth = 1; depth <= getMaxDepth(); depth++)
        createDockWidgets(depth);
    
    Application::processEvents();

    removePlaceHolderDockWidgets();

    Application::processEvents();

    DockArea::setSplitterWidgetSizes(_dockManager->getRootSplitter());

    loadViewPluginDockWidgets();
}

void DockArea::sanitizeHierarchy()
{
    for (auto& child : _children) {
        child.setParent(this);
        child.sanitizeHierarchy();
    }
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

void DockArea::buildTreeFromDocking(QWidget* widget)
{
    auto splitter = qobject_cast<QSplitter*>(widget);

    if (splitter) {
        setOrientation(splitter->orientation());

        float size = 0.f;

        switch (splitter->orientation()) {
            case Qt::Horizontal:
                size = static_cast<float>(splitter->parentWidget()->width());
                break;

            case Qt::Vertical:
                size = static_cast<float>(splitter->parentWidget()->height());
                break;
        }

        SplitterRatios splitterRatios;

        for (const auto& splitterRatio : splitter->sizes())
            splitterRatios << static_cast<float>(splitterRatio) / size;

        setSplitterRatios(splitterRatios);

        DockAreas children;

        for (int i = 0; i < splitter->count(); ++i) {
            DockArea childDockArea(_dockManager, _depth + 1);

            childDockArea.buildTreeFromDocking(splitter->widget(i));

            children << childDockArea;
        }

        setChildren(children);
    }
    else {
        auto dockAreaWidget = qobject_cast<CDockAreaWidget*>(widget);

        if (!dockAreaWidget)
            return;

        setCentral(dockAreaWidget->isCentralWidgetArea());

        for (auto adsDockWidget : dockAreaWidget->dockWidgets()) {
            auto dockWidget = dynamic_cast<DockWidget*>(adsDockWidget);

            if (dockWidget)
                _dockWidgets << dockWidget;
        }
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

void DockArea::setSplitterWidgetSizes(QWidget* widget)
{
    Q_ASSERT(widget != nullptr);

    auto splitter = qobject_cast<QSplitter*>(widget);

    if (splitter) {
        if (splitter->count() >= 2) {
            float size = 0.f;

            switch (splitter->orientation()) {
                case Qt::Horizontal:
                    size = static_cast<float>(splitter->width());
                    break;

                case Qt::Vertical:
                    size = static_cast<float>(splitter->height());
                    break;
            }

            QVector<std::int32_t> splitterSizes;

            //qDebug() << splitter->orientation() << splitter->sizes() << splitter->property("SplitterRatios").toList() << splitter->count();

            for (const auto& splitterRatio : splitter->property("SplitterRatios").toList())
                splitterSizes << size * splitterRatio.toFloat();

            if (splitterSizes.count() == splitter->count()) {
                if (!splitterSizes.isEmpty())
                    splitter->setSizes(splitterSizes);
            }
            else {
                qCritical() << "Unable to set dock area splitter size(s), there is a mismatch between the number of sizes recorded and the splitter count.";
            }
        }
            
        for (int i = 0; i < splitter->count(); ++i)
            setSplitterWidgetSizes(splitter->widget(i));
    }
}
