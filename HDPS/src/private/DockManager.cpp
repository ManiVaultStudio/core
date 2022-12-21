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
    CDockManager::setConfigFlag(CDockManager::AllTabsHaveCloseButton, true);
}

DockManager::~DockManager()
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << objectName();
#endif
}

ViewPluginDockWidgets DockManager::getViewPluginDockWidgets()
{
    ViewPluginDockWidgets viewPluginDockWidgets;

    for (auto dockWidget : dockWidgets()) {
        auto viewPluginDockWidget = dynamic_cast<ViewPluginDockWidget*>(dockWidget);

        if (viewPluginDockWidget == nullptr)
            continue;

        viewPluginDockWidgets << viewPluginDockWidget;
    }

    return viewPluginDockWidgets;
}

const ViewPluginDockWidgets DockManager::getViewPluginDockWidgets() const
{
    return const_cast<DockManager*>(this)->getViewPluginDockWidgets();
}

ads::CDockAreaWidget* DockManager::findDockAreaWidget(QWidget* widget)
{
    for (auto dockWidget : dockWidgets())
        if (dockWidget->widget() == widget)
            return dockWidget->dockAreaWidget();

    return nullptr;
}

void DockManager::removeViewPluginDockWidget(ViewPlugin* viewPlugin)
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << viewPlugin->getGuiName();
#endif

    Q_ASSERT(viewPlugin != nullptr);

    for (auto dockWidget : dockWidgets()) {
        auto viewPluginDockWidget = dynamic_cast<ViewPluginDockWidget*>(dockWidget);

        if (viewPluginDockWidget == nullptr)
            continue;

        if (viewPlugin == viewPluginDockWidget->getViewPlugin())
            removeDockWidget(viewPluginDockWidget);
    }
}

void DockManager::fromVariantMap(const QVariantMap& variantMap)
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << objectName();
#endif

    variantMapMustContain(variantMap, "State");
    variantMapMustContain(variantMap, "ViewPluginDockWidgets");

    hide();
    {
        //for (auto viewPluginDockWidget : _viewPluginDockWidgets)
        //    removeDockWidget(viewPluginDockWidget);

//        _viewPluginDockWidgets.clear();

        for (auto viewPluginDockWidgetVariant : variantMap["ViewPluginDockWidgets"].toList()) {
            //auto dockWidget = new CDockWidget("Test");
            //dockWidget->setObjectName(viewPluginDockWidgetVariant.toMap()["ID"].toString());
            //addDockWidget(RightDockWidgetArea, dockWidget);
            addDockWidget(RightDockWidgetArea, new ViewPluginDockWidget(viewPluginDockWidgetVariant.toMap()));
        }
            

        if (!restoreState(QByteArray::fromBase64(variantMap["State"].toString().toUtf8()), variantMap["Version"].toInt()))
            qCritical() << "Unable to restore state from" << objectName();
    }
    show();
}

QVariantMap DockManager::toVariantMap() const
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << objectName();
#endif

    QVariantList viewPluginDockWidgetsList;

    for (auto viewPluginDockWidget : getViewPluginDockWidgets())
        viewPluginDockWidgetsList << viewPluginDockWidget->toVariantMap();

    return {
        { "State", QVariant::fromValue(saveState().toBase64()) },
        { "ViewPluginDockWidgets", viewPluginDockWidgetsList }
    };
}

void DockManager::reset()
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << objectName();
#endif

    for (auto viewPluginDockWidget : getViewPluginDockWidgets())
        removeDockWidget(viewPluginDockWidget);
}
