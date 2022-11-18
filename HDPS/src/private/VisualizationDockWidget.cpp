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
    _centralDockArea(nullptr),
    _centralDockWidget("CentralDockWidget"),
    _logoWidget(),
    _lastDockAreaWidget(nullptr)
{
    _dockManager.setConfigFlag(CDockManager::FocusHighlighting, false);

    setObjectName("VisualizationDockWidget");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setWidget(&_dockManager, eInsertMode::ForceNoScrollArea);

    _centralDockWidget.setWidget(&_logoWidget);

    _centralDockArea = _dockManager.setCentralWidget(&_centralDockWidget);

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

    _viewPluginDockWidgets << viewPluginDockWidget;

    //if (_lastDockAreaWidget == nullptr || getNumberOfOpenViewPluginDockWidgets() == 0)
    //    _lastDockAreaWidget = _dockManager.addDockWidget(CenterDockWidgetArea, viewPluginDockWidget);
    //else
    //    _lastDockAreaWidget = _dockManager.addDockWidget(RightDockWidgetArea, viewPluginDockWidget, _lastDockAreaWidget);

    auto dockAreaWidget = findDockAreaWidget(dockToViewPlugin);

    _lastDockAreaWidget = _dockManager.addDockWidget(static_cast<DockWidgetArea>(dockArea), viewPluginDockWidget, dockAreaWidget);

    const auto lastDockAreaWidget = _lastDockAreaWidget;

    connect(_lastDockAreaWidget, &CDockAreaWidget::destroyed, this, [this, lastDockAreaWidget]() -> void {
        if (lastDockAreaWidget == _lastDockAreaWidget)
            _lastDockAreaWidget = nullptr;
    });

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
    Q_ASSERT(_centralDockArea != nullptr);

//#ifdef VISUALIZATION_DOCK_WIDGET_VERBOSE
//    qDebug() << __FUNCTION__ << getNumberOfOpenViewPluginDockWidgets();
//#endif
    
    //if (getNumberOfOpenViewPluginDockWidgets() == 0) {
    //    _centralDockArea->setAllowedAreas(DockWidgetArea::AllDockAreas);
    //    _centralDockWidget.toggleView(true);
    //}
    //else {
    //    _centralDockArea->setAllowedAreas(DockWidgetArea::AllDockAreas);
    //    _centralDockWidget.toggleView(false);
    //}
}

std::int32_t VisualizationDockWidget::getNumberOfOpenViewPluginDockWidgets() const
{
//#ifdef VISUALIZATION_DOCK_WIDGET_VERBOSE
//    qDebug() << __FUNCTION__ << _viewPluginDockWidgets;
//#endif

    std::int32_t numberOfOpenViewPluginDockWidgets = 0;

    for (auto viewPluginDockWidget : _viewPluginDockWidgets)
        if (!viewPluginDockWidget->isClosed())
            numberOfOpenViewPluginDockWidgets++;

    return numberOfOpenViewPluginDockWidgets;
}

void VisualizationDockWidget::reset()
{
//#ifdef VISUALIZATION_DOCK_WIDGET_VERBOSE
//    qDebug() << __FUNCTION__;
//#endif

    for (auto viewPluginDockWidget : _viewPluginDockWidgets)
        _dockManager.removeDockWidget(viewPluginDockWidget);

    _viewPluginDockWidgets.clear();

    updateCentralWidget();
}

ads::CDockAreaWidget* VisualizationDockWidget::findDockAreaWidget(ViewPlugin* viewPlugin)
{
    return _dockManager.findDockAreaWidget(&viewPlugin->getWidget());
}