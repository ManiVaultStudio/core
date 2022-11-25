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

DockManager::DockManager(QWidget* parent /*= nullptr*/) :
    CDockManager(parent),
    Serializable("Dock manager"),
    _centralDockAreaWidget(nullptr),
    _centralDockWidget(this),
    _viewPluginDockWidgets()
{
    _centralDockWidget.setObjectName("CentralDockWidget");

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
    CDockManager::setConfigFlag(CDockManager::AllTabsHaveCloseButton, true);

    connect(this, &CDockManager::dockWidgetAdded, this, &DockManager::dockWidgetAdded);
    connect(this, &CDockManager::dockWidgetAboutToBeRemoved, this, &DockManager::dockWidgetAboutToBeRemoved);
}

ViewPluginDockWidgets DockManager::getViewPluginDockWidgets() const
{
    return _viewPluginDockWidgets;
}

QSplitter* DockManager::getRootSplitter() const
{
    return CDockManager::rootSplitter();
}

ads::CDockAreaWidget* DockManager::findDockAreaWidget(QWidget* widget)
{
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
    qDebug() << __FUNCTION__ << objectName();
#endif

    reset();

    DockArea rootDockArea(this);

    rootDockArea.fromVariantMap(variantMap["RootArea"].toMap());
    rootDockArea.applyDocking();

#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << rootDockArea;
#endif
}

QVariantMap DockManager::toVariantMap() const
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << objectName();
#endif

    DockArea rootDockArea(const_cast<DockManager*>(this), 0);

    rootDockArea.buildTreeFromDocking(rootSplitter());

    return {
        { "RootArea", rootDockArea.toVariantMap() }
    };
}

void DockManager::reset()
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << objectName();
#endif

    for (auto viewPluginDockWidget : _viewPluginDockWidgets)
        removeDockWidget(viewPluginDockWidget);
}

void DockManager::dockWidgetAdded(ads::CDockWidget* dockWidget)
{
    Q_ASSERT(dockWidget != nullptr);

    auto viewPluginDockWidget = dynamic_cast<ViewPluginDockWidget*>(dockWidget);

    if (viewPluginDockWidget == nullptr)
        return;

    _viewPluginDockWidgets << viewPluginDockWidget;
}

void DockManager::dockWidgetAboutToBeRemoved(ads::CDockWidget* dockWidget)
{
    Q_ASSERT(dockWidget != nullptr);

    auto viewPluginDockWidget = dynamic_cast<ViewPluginDockWidget*>(dockWidget);

    if (viewPluginDockWidget == nullptr)
        return;

    if (_viewPluginDockWidgets.contains(viewPluginDockWidget))
        _viewPluginDockWidgets.removeOne(viewPluginDockWidget);
}