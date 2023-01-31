#include "DockManager.h"
#include "ViewPluginDockWidget.h"

#include <ViewPlugin.h>

#include <CoreInterface.h>
#include <AbstractPluginManager.h>

#include <util/Serialization.h>
#include <widgets/InfoWidget.h>

#include <DockAreaWidget.h> 

#include <QSplitter>

#ifdef _DEBUG
    #define DOCK_MANAGER_VERBOSE
#endif

using namespace ads;

using namespace hdps;
using namespace hdps::plugin;
using namespace hdps::gui;
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
    CDockManager::setConfigFlag(CDockManager::FocusHighlighting, false);
    CDockManager::setConfigFlag(CDockManager::EqualSplitOnInsertion, true);
    CDockManager::setConfigFlag(CDockManager::DockAreaHasUndockButton, true);
    CDockManager::setConfigFlag(CDockManager::DockAreaHasCloseButton, false);
    CDockManager::setConfigFlag(CDockManager::DockAreaHasUndockButton, false);
    CDockManager::setConfigFlag(CDockManager::DockAreaHasTabsMenuButton, true);
    CDockManager::setConfigFlag(CDockManager::DockAreaDynamicTabsMenuButtonVisibility, false);
    CDockManager::setConfigFlag(CDockManager::AllTabsHaveCloseButton, true);
    
    //setAutoHideConfigFlag(AutoHideFeatureEnabled);
    //setAutoHideConfigFlag(DockAreaHasAutoHideButton);
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

ads::CDockAreaWidget* DockManager::findDockAreaWidget(hdps::plugin::ViewPlugin* viewPlugin)
{
    if (viewPlugin == nullptr)
        return nullptr;

    return findDockAreaWidget(&viewPlugin->getWidget());
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

void DockManager::reset()
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << objectName();
#endif

    for (auto viewPluginDockWidget : getViewPluginDockWidgets())
        removeDockWidget(viewPluginDockWidget);
}

void DockManager::fromVariantMap(const QVariantMap& variantMap)
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << objectName();
#endif

    Serializable::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "State");
    variantMapMustContain(variantMap, "ViewPluginDockWidgets");

    hide();
    {
        for (auto viewPluginDockWidgetVariant : variantMap["ViewPluginDockWidgets"].toList()) {
            const auto viewPluginMap    = viewPluginDockWidgetVariant.toMap()["ViewPlugin"].toMap();
            const auto pluginKind       = viewPluginMap["Kind"].toString();
            const auto pluginMap        = viewPluginMap["Plugin"].toMap();

            if (plugins().isPluginLoaded(pluginKind)) {
                addDockWidget(RightDockWidgetArea, new ViewPluginDockWidget(viewPluginDockWidgetVariant.toMap()));
            } else {
                auto notLoadedDockWidget    = new CDockWidget(QString("%1 (not loaded)").arg(viewPluginMap["GuiName"].toMap()["Value"].toString()));
                auto notLoadedInfoWidget    = new InfoWidget(this, Application::getIconFont("FontAwesome").getIcon("exclamation-circle"), "View not loaded", QString("We were unable to load the %1 because the plugin is not loaded properly.\nThe workspace might not behave as expected, please ensure the required plugin is loaded properly...").arg(pluginKind));

                notLoadedInfoWidget->setColor(Qt::gray);
                notLoadedDockWidget->setWidget(notLoadedInfoWidget);
                notLoadedDockWidget->setObjectName(viewPluginDockWidgetVariant.toMap()["ID"].toString());

                addDockWidget(RightDockWidgetArea, notLoadedDockWidget);
            }
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

    auto variantMap = Serializable::toVariantMap();

    QVariantList viewPluginDockWidgetsList;

    for (auto viewPluginDockWidget : getViewPluginDockWidgets())
        viewPluginDockWidgetsList << viewPluginDockWidget->toVariantMap();

    variantMap.insert({
        { "State", QVariant::fromValue(saveState().toBase64()) },
        { "ViewPluginDockWidgets", viewPluginDockWidgetsList }
    });

    return variantMap;
}
