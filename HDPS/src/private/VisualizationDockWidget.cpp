#include "VisualizationDockWidget.h"

#include <ViewPlugin.h>

#include <QVBoxLayout>

#include "DockAreaWidget.h"

#ifdef _DEBUG
    #define VISUALIZATION_DOCK_WIDGET_VERBOSE
#endif

using namespace ads;

using namespace hdps::plugin;

VisualizationDockWidget::VisualizationDockWidget(QWidget* parent /*= nullptr*/) :
    ads::CDockWidget("Visualization", parent),
    _widget(),
    _dockManager(new CDockManager(&_widget)),
    _centralDockArea(nullptr),
    _centralDockWidget(""),
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

ads::CDockManager& VisualizationDockWidget::getDockManager()
{
    return _dockManager;
}

void VisualizationDockWidget::addViewPlugin(ads::CDockWidget* viewPluginDockWidget)
{
    Q_ASSERT(viewPluginDockWidget != nullptr);

    _viewPluginDockWidgets << viewPluginDockWidget;

    if (_lastDockAreaWidget == nullptr || getNumberOfOpenViewPluginDockWidgets() == 0)
        _lastDockAreaWidget = _dockManager.addDockWidget(CenterDockWidgetArea, viewPluginDockWidget);
    else
        _lastDockAreaWidget = _dockManager.addDockWidget(RightDockWidgetArea, viewPluginDockWidget, _lastDockAreaWidget);

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

void VisualizationDockWidget::updateCentralWidget()
{
    Q_ASSERT(_centralDockArea != nullptr);

#ifdef VISUALIZATION_DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__ << getNumberOfOpenViewPluginDockWidgets();
#endif
    
    if (getNumberOfOpenViewPluginDockWidgets() == 0) {
        _centralDockArea->setAllowedAreas(DockWidgetArea::AllDockAreas);
        _centralDockWidget.toggleView(true);
    }
    else {
        _centralDockArea->setAllowedAreas(DockWidgetArea::AllDockAreas);
        _centralDockWidget.toggleView(false);
    }
}

std::int32_t VisualizationDockWidget::getNumberOfOpenViewPluginDockWidgets() const
{
    std::int32_t numberOfOpenViewPluginDockWidgets = 0;

    for (auto viewPluginDockWidget : _viewPluginDockWidgets)
        if (!viewPluginDockWidget->isClosed())
            numberOfOpenViewPluginDockWidgets++;

    return numberOfOpenViewPluginDockWidgets;
}