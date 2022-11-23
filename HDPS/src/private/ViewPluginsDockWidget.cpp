#include "ViewPluginsDockWidget.h"

#include <util/Serialization.h>

#include "DockAreaWidget.h"

#include <QVBoxLayout>

#ifdef _DEBUG
    #define VIEW_PLUGINS_DOCK_WIDGET_VERBOSE
#endif

using namespace ads;

using namespace hdps::plugin;
using namespace hdps::util;
using namespace hdps::gui;

ViewPluginsDockWidget::ViewPluginsDockWidget(QWidget* parent /*= nullptr*/) :
    CentralDockWidget(parent),
    _widget(),
    _dockManager(&_widget),   
    _logoWidget()
{
    _dockManager.setConfigFlag(CDockManager::FocusHighlighting, false);
    _dockManager.setObjectName("ViewPluginsDockManager");
    _dockManager.setCentralWidget(&_logoWidget);

    setObjectName("ViewPluginsDockWidget");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setWidget(&_dockManager, eInsertMode::ForceNoScrollArea);

    connect(&_dockManager, &CDockManager::dockAreasAdded, this, &ViewPluginsDockWidget::updateCentralWidget);
    connect(&_dockManager, &CDockManager::dockAreasRemoved, this, &ViewPluginsDockWidget::updateCentralWidget);
}

QString ViewPluginsDockWidget::getTypeString() const
{
    return "ViewPluginsDockWidget";
}

DockManager& ViewPluginsDockWidget::getDockManager()
{
    return _dockManager;
}

const DockManager& ViewPluginsDockWidget::getDockManager() const
{
    return const_cast<ViewPluginsDockWidget*>(this)->_dockManager;
}

void ViewPluginsDockWidget::addViewPlugin(ViewPluginDockWidget* viewPluginDockWidget, hdps::plugin::ViewPlugin* dockToViewPlugin, hdps::gui::DockAreaFlag dockArea)
{
    Q_ASSERT(viewPluginDockWidget != nullptr);

    auto dockAreaWidget = findDockAreaWidget(dockToViewPlugin);

#ifdef VIEW_PLUGINS_DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__ << dockAreaWidget << dockAreaMap.key(dockArea);
#endif

    _dockManager.addDockWidget(static_cast<DockWidgetArea>(dockArea), viewPluginDockWidget, dockAreaWidget);

    connect(viewPluginDockWidget->dockAreaWidget(), &CDockAreaWidget::currentChanged, [this](int index) {
        updateCentralWidget();
    });

    connect(viewPluginDockWidget, &CDockWidget::viewToggled, [this, viewPluginDockWidget](bool toggled) {
        updateCentralWidget();
    });

    connect(viewPluginDockWidget, &CDockWidget::topLevelChanged, [this, viewPluginDockWidget](bool topLevel) {
        updateCentralWidget();
    });

    updateCentralWidget();
}

void ViewPluginsDockWidget::updateCentralWidget()
{
    if (_dockManager.getCentralDockAreaWidget() == nullptr)
        return;

#ifdef VIEW_PLUGINS_DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__ << getNumberOfOpenViewPluginDockWidgets();
#endif
    
    _dockManager.getCentralDockWidget()->toggleView(getNumberOfOpenViewPluginDockWidgets() == 0);
}

std::int32_t ViewPluginsDockWidget::getNumberOfOpenViewPluginDockWidgets() const
{
    std::int32_t numberOfOpenViewPluginDockWidgets = 0;
    
    for (auto dockWidget : _dockManager.dockWidgets())
        if (dynamic_cast<ViewPluginDockWidget*>(dockWidget) && !dockWidget->isClosed())
            numberOfOpenViewPluginDockWidgets++;

    return numberOfOpenViewPluginDockWidgets;
}

ads::CDockAreaWidget* ViewPluginsDockWidget::findDockAreaWidget(ViewPlugin* viewPlugin)
{
    return _dockManager.findDockAreaWidget(&viewPlugin->getWidget());
}