// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DockManager.h"
#include "ViewPluginDockWidget.h"
#include "ViewPluginsDockWidget.h"

#include <AbstractPluginManager.h>
#include <CoreInterface.h>
#include <ViewPlugin.h>

#include <util/Serialization.h>
#include <widgets/InfoWidget.h>

#include <DockAreaWidget.h> 
#include <QLoggingCategory> 

#ifdef _DEBUG
    #define DOCK_MANAGER_VERBOSE
#endif

using namespace ads;

using namespace mv;
using namespace mv::plugin;
using namespace mv::gui;
using namespace mv::util;

DockManager::DockManager(const QString& name, QWidget* parent /*= nullptr*/) :
    CDockManager(parent),
    Serializable("Dock manager"),
    _name(name),
    _serializationTask(nullptr),
    _layoutTask(this, "Load " + _name.toLower() + " layout")
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
    CDockManager::setConfigFlag(CDockManager::FloatingContainerHasWidgetIcon, true);
}

DockManager::~DockManager()
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << objectName();
#endif

    reset();
}

DockManager::ViewPluginDockWidgets DockManager::getViewPluginDockWidgets(bool floating)
{
    ViewPluginDockWidgets viewPluginDockWidgets;

    for (auto dockWidget : dockWidgets()) {
        if (auto viewPluginDockWidget = dynamic_cast<ViewPluginDockWidget*>(dockWidget))
            viewPluginDockWidgets.push_back(viewPluginDockWidget);
    }
     
    if (floating)  {
        for (auto floatingDockContainer : floatingWidgets())
            for (auto dockWidget : floatingDockContainer->dockWidgets())
                if (auto viewPluginDockWidget = dynamic_cast<ViewPluginDockWidget*>(dockWidget))
                    viewPluginDockWidgets.push_back(viewPluginDockWidget);
    }

    return viewPluginDockWidgets;
}

DockManager::ViewPluginDockWidgets DockManager::getViewPluginDockWidgets(bool floating) const
{
    return const_cast<DockManager*>(this)->getViewPluginDockWidgets(floating);
}

ViewPluginDockWidget* DockManager::findViewPluginDockWidget(const mv::plugin::ViewPlugin* viewPlugin) const
{
    Q_ASSERT(viewPlugin);

    if (!viewPlugin)
        return nullptr;

    for (auto dockWidget : dockWidgets()) {
        if (viewPlugin->getId() == dockWidget->property("ViewPluginId").toString())
            return dynamic_cast<ViewPluginDockWidget*>(dockWidget);
    }

    return nullptr;
}

CDockAreaWidget* DockManager::findDockAreaWidget(const ViewPlugin* viewPlugin) const
{
    Q_ASSERT(viewPlugin);

    if (!viewPlugin)
        return nullptr;

    for (auto dockWidget : dockWidgets()) {
        if (viewPlugin->getId() == dockWidget->property("ViewPluginId").toString())
            return dockWidget->dockAreaWidget();
    }

    return nullptr;
}

void DockManager::removeViewPluginDockWidget(ViewPlugin* viewPlugin)
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << viewPlugin->getGuiName();
#endif

    Q_ASSERT(viewPlugin);

    if (!viewPlugin)
        return;

    if (auto viewPluginDockWidget = findViewPluginDockWidget(viewPlugin))
        removeViewPluginDockWidget(viewPluginDockWidget);
}

void DockManager::reset()
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << objectName();
#endif

    for (auto viewPluginDockWidget : getViewPluginDockWidgets())
        removeViewPluginDockWidget(viewPluginDockWidget);
}

void DockManager::addViewPluginDockWidget(ads::DockWidgetArea area, ads::CDockWidget* dockWidget, ads::CDockAreaWidget* dockAreaWidget)
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << objectName();
#endif
    
    CDockManager::addDockWidget(area, dockWidget, dockAreaWidget);
}

void DockManager::removeViewPluginDockWidget(ViewPluginDockWidget* viewPluginDockWidget)
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << objectName();
#endif

    CDockManager::removeDockWidget((DockWidget*)viewPluginDockWidget);

	viewPluginDockWidget->deleteLater();
}

QWidget* DockManager::getWidget()
{
    return this;
}

void DockManager::fromVariantMap(const QVariantMap& variantMap)
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << objectName();
#endif

    Serializable::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "State");
    variantMapMustContain(variantMap, "ViewPluginDockWidgets");

    _layoutTask.setName("Load " + _name.toLower() + " layout");

    hide();
    {
        const auto viewPluginDockWidgetsList = variantMap["ViewPluginDockWidgets"].toList();

        for (const auto& viewPluginDockWidgetVariant : viewPluginDockWidgetsList)
            ViewPluginDockWidget::preRegisterSerializationTask(this, viewPluginDockWidgetVariant.toMap()["ID"].toString(), this);

        for (const auto& viewPluginDockWidgetVariant : viewPluginDockWidgetsList) {
            const auto viewPluginMap    = viewPluginDockWidgetVariant.toMap()["ViewPlugin"].toMap();
            const auto pluginKind       = viewPluginMap["Kind"].toString();
            const auto pluginMap        = viewPluginMap["Plugin"].toMap();
            const auto guiName          = viewPluginMap["GuiName"].toMap()["Value"].toString();

            if (plugins().isPluginLoaded(pluginKind)) {
                addViewPluginDockWidget(RightDockWidgetArea, new ViewPluginDockWidget(viewPluginDockWidgetVariant.toMap()));
            } else {
                auto notLoadedDockWidget    = new CDockWidget(QString("%1 (not loaded)").arg(guiName));
                auto notLoadedInfoWidget    = new InfoWidget(this, Application::getIconFont("FontAwesome").getIcon("exclamation-circle"), "View not loaded", QString("We were unable to load the %1 because the plugin is not loaded properly.\nThe workspace might not behave as expected, please ensure the required plugin is loaded properly...").arg(pluginKind));

                notLoadedDockWidget->setWidget(notLoadedInfoWidget);
                notLoadedDockWidget->setObjectName(viewPluginDockWidgetVariant.toMap()["ID"].toString());

                addViewPluginDockWidget(RightDockWidgetArea, notLoadedDockWidget);
            }

            QCoreApplication::processEvents();
        }

        _layoutTask.setRunning();
        {
            if (!restoreState(QByteArray::fromBase64(variantMap["State"].toString().toUtf8()), variantMap["Version"].toInt()))
                qCritical() << "Unable to restore state from" << objectName();
        }
        _layoutTask.setFinished();

        _serializationTask->setFinished();

        ViewPluginDockWidget::removeAllSerializationTasks();
    }
    show();
}

QVariantMap DockManager::toVariantMap() const
{
#ifdef DOCK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << objectName();
#endif

    const_cast<DockManager*>(this)->_layoutTask.setName("Save " + _name.toLower() + " layout");

    _serializationTask->setEnabled(true);

    for (const auto& viewPluginDockWidget : getViewPluginDockWidgets())
        ViewPluginDockWidget::preRegisterSerializationTask(const_cast<DockManager*>(this), viewPluginDockWidget->getId(), const_cast<DockManager*>(this));

    auto variantMap = Serializable::toVariantMap();

    QVariantList viewPluginDockWidgetsList;

    for (const auto& viewPluginDockWidget : getViewPluginDockWidgets())
        viewPluginDockWidgetsList << viewPluginDockWidget->toVariantMap();

    const_cast<DockManager*>(this)->_layoutTask.setRunning();
    {
        variantMap.insert({
            { "State", QVariant::fromValue(saveState().toBase64()) },
            { "ViewPluginDockWidgets", viewPluginDockWidgetsList }
        });
    }
    const_cast<DockManager*>(this)->_layoutTask.setFinished();

    _serializationTask->setFinished();

    ViewPluginDockWidget::removeAllSerializationTasks();

    return variantMap;
}

Task* DockManager::getSerializationTask() const
{
    return _serializationTask;
}

void DockManager::setSerializationTask(mv::Task* serializationTask)
{
    Q_ASSERT(serializationTask != nullptr);

    if (serializationTask == nullptr)
        return;

    if (serializationTask == _serializationTask)
        return;

    _serializationTask = serializationTask;

    _layoutTask.setParentTask(_serializationTask);
}
