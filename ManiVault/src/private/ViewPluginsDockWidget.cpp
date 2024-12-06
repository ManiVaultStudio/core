// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginsDockWidget.h"

#include <CoreInterface.h>
#include <AbstractPluginManager.h>

#include "DockAreaWidget.h"

#include <QVBoxLayout>

#ifdef _DEBUG
    //#define VIEW_PLUGINS_DOCK_WIDGET_VERBOSE
#endif

using namespace ads;

using namespace mv;
using namespace mv::plugin;
using namespace mv::util;
using namespace mv::gui;

QPointer<DockManager> ViewPluginsDockWidget::dockManager = QPointer<DockManager>();

ViewPluginsDockWidget::ViewPluginsDockWidget(const QPointer<DockManager>& dockManager, QWidget* parent /*= nullptr*/) :
    CDockWidget("View plugins", parent),
    _centralDockWidget("Central dock widget")
{
    ViewPluginsDockWidget::dockManager = dockManager;

    auto widget = new QWidget();
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(dockManager);

    widget->setLayout(layout);

    setWidget(widget);

    _centralDockWidget.setWidget(&_logoWidget);

    dockManager->setCentralWidget(&_centralDockWidget);

    connect(dockManager, &CDockManager::dockAreasAdded, this, &ViewPluginsDockWidget::updateCentralWidget);
    connect(dockManager, &CDockManager::dockAreasRemoved, this, &ViewPluginsDockWidget::updateCentralWidget);
    connect(dockManager, &CDockManager::dockWidgetAdded, this, &ViewPluginsDockWidget::dockWidgetAdded);
    connect(dockManager, &CDockManager::dockWidgetAboutToBeRemoved, this, &ViewPluginsDockWidget::dockWidgetAboutToBeRemoved);
    connect(dockManager, &CDockManager::focusedDockWidgetChanged, this, &ViewPluginsDockWidget::updateCentralWidget);

    connect(&plugins(), &AbstractPluginManager::pluginDestroyed, this, &ViewPluginsDockWidget::updateCentralWidget);
}

void ViewPluginsDockWidget::updateCentralWidget()
{
#ifdef VIEW_PLUGINS_DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif
    
    std::int32_t numberOfVisibleNonCentralDockWidgets = 0;

    for (auto dockWidget : dockManager->dockWidgets())
        if (!dockWidget->isCentralWidget() && !dockWidget->isClosed())
            numberOfVisibleNonCentralDockWidgets++;

    _centralDockWidget.toggleView(numberOfVisibleNonCentralDockWidgets == 0);
}

void ViewPluginsDockWidget::dockWidgetAdded(const ads::CDockWidget* dockWidget)
{
    Q_ASSERT(dockWidget != nullptr);

    if (!dockWidget)
        return;

    connect(dockWidget, &CDockWidget::viewToggled, this, [this](bool toggled) {
        updateCentralWidget();
    });

    connect(dockWidget->dockAreaWidget(), &CDockAreaWidget::currentChanged, this, [this](int index) {
        updateCentralWidget();
    });

    connect(dockWidget, &CDockWidget::topLevelChanged, this, [this](bool topLevel) {
        updateCentralWidget();
    });
}

void ViewPluginsDockWidget::dockWidgetAboutToBeRemoved(const ads::CDockWidget* dockWidget)
{
    Q_ASSERT(dockWidget != nullptr);

    if (!dockWidget)
        return;

    disconnect(dockWidget, &CDockWidget::viewToggled, this, nullptr);
    disconnect(dockWidget->dockAreaWidget(), &CDockAreaWidget::currentChanged, this, nullptr);
    disconnect(dockWidget, &CDockWidget::topLevelChanged, this, nullptr);
}

void ViewPluginsDockWidget::isolate(const plugin::ViewPlugin* viewPlugin, bool isolate)
{
    if (viewPlugin->isSystemViewPlugin())
        return;

    if (isolate) {
        cacheVisibility();

        for (auto viewPluginDockWidget : ViewPluginsDockWidget::dockManager->getViewPluginDockWidgets()) {
            if (viewPlugin != viewPluginDockWidget->getViewPlugin())
                viewPluginDockWidget->toggleView(false);
        }
    }
    else {
        restoreVisibility();
    }
}

void ViewPluginsDockWidget::cacheVisibility()
{
    for (auto viewPluginDockWidget : ViewPluginsDockWidget::dockManager->getViewPluginDockWidgets())
        viewPluginDockWidget->cacheVisibility();
}

void ViewPluginsDockWidget::restoreVisibility()
{
    for (auto viewPluginDockWidget : ViewPluginsDockWidget::dockManager->getViewPluginDockWidgets())
        viewPluginDockWidget->restoreVisibility();
}
