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
    _centralDockWidget(this)
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
    DockArea rootDockArea(const_cast<DockManager*>(this), 0);

    rootDockArea.buildTreeFromDocking(rootSplitter());

    return {
        { "RootArea", rootDockArea.toVariantMap() }
    };
}

void DockManager::reset()
{
    for (auto dockWidget : dockWidgetsMap().values())
        if (dockWidget != &_centralDockWidget)
            removeDockWidget(dockWidget);
}
