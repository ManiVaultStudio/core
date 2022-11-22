#include "VisualizationDockWidget.h"

#include <util/Serialization.h>

#include "DockAreaWidget.h"

#include <QVBoxLayout>

#ifdef _DEBUG
    #define VISUALIZATION_DOCK_WIDGET_VERBOSE
#endif

using namespace ads;

using namespace hdps::plugin;
using namespace hdps::util;
using namespace hdps::gui;

VisualizationDockWidget::VisualizationDockWidget(QWidget* parent /*= nullptr*/) :
    DockWidget("Visualization", parent),
    _widget(),
    _dockManager(&_widget),   
    _logoWidget()
{
    _dockManager.setConfigFlag(CDockManager::FocusHighlighting, false);
    _dockManager.setObjectName("Visualization");
    _dockManager.setCentralWidget(&_logoWidget);

    setObjectName("VisualizationDockWidget");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setWidget(&_dockManager, eInsertMode::ForceNoScrollArea);

    reset();

    connect(&_dockManager, &CDockManager::dockAreasAdded, this, &VisualizationDockWidget::updateCentralWidget);
    connect(&_dockManager, &CDockManager::dockAreasRemoved, this, &VisualizationDockWidget::updateCentralWidget);
}

DockManager& VisualizationDockWidget::getDockManager()
{
    return _dockManager;
}

const DockManager& VisualizationDockWidget::getDockManager() const
{
    return const_cast<VisualizationDockWidget*>(this)->_dockManager;
}

void VisualizationDockWidget::addViewPlugin(ViewPluginDockWidget* viewPluginDockWidget, hdps::plugin::ViewPlugin* dockToViewPlugin, hdps::gui::DockAreaFlag dockArea)
{
    Q_ASSERT(viewPluginDockWidget != nullptr);

    auto dockAreaWidget = findDockAreaWidget(dockToViewPlugin);

#ifdef VISUALIZATION_DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__ << dockAreaWidget << dockAreaMap.key(dockArea);
#endif

    _viewPluginDockWidgets << viewPluginDockWidget;

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

void VisualizationDockWidget::fromVariantMap(const QVariantMap& variantMap)
{
    _dockManager.setCentralWidget(nullptr);

    reset();

    _dockManager.fromVariantMap(variantMap["Docking"].toMap());

    updateCentralWidget();
}

QVariantMap VisualizationDockWidget::toVariantMap() const
{
    QVariantMap variantMap = DockWidget::toVariantMap();

    variantMap["Docking"] = _dockManager.toVariantMap();

    return variantMap;
}

void VisualizationDockWidget::updateCentralWidget()
{
    if (_dockManager.getCentralDockAreaWidget() == nullptr)
        return;

#ifdef VISUALIZATION_DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__ << getNumberOfOpenViewPluginDockWidgets();
#endif
    
    _dockManager.getCentralDockWidget()->toggleView(getNumberOfOpenViewPluginDockWidgets() == 0);
}

std::int32_t VisualizationDockWidget::getNumberOfOpenViewPluginDockWidgets() const
{
    std::int32_t numberOfOpenViewPluginDockWidgets = 0;

    for (auto viewPluginDockWidget : _viewPluginDockWidgets)
        if (!viewPluginDockWidget->isClosed())
            numberOfOpenViewPluginDockWidgets++;

    return numberOfOpenViewPluginDockWidgets;
}

void VisualizationDockWidget::reset()
{
#ifdef VISUALIZATION_DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    //_dockManager.reset();
    /*
    for (auto viewPluginDockWidget : _viewPluginDockWidgets) {
        _dockManager.removeDockWidget(viewPluginDockWidget);

        qDebug() << viewPluginDockWidget->windowTitle();
    }
    */  

    _viewPluginDockWidgets.clear();

    //updateCentralWidget();
}

ads::CDockAreaWidget* VisualizationDockWidget::findDockAreaWidget(ViewPlugin* viewPlugin)
{
    return _dockManager.findDockAreaWidget(&viewPlugin->getWidget());
}