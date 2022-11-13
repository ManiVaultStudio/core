#include "VisualizationDockWidget.h"

#include <ViewPlugin.h>

#include <QVBoxLayout>

#include "DockAreaWidget.h"

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

    if (_lastDockAreaWidget == nullptr)
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

    bool hasOpenViewPluginDockWidgets = false;

    for (auto viewPluginDockWidget : _viewPluginDockWidgets) {
        if (!viewPluginDockWidget->isClosed()) {
            hasOpenViewPluginDockWidgets = true;
            break;
        }
    }

    if (!hasOpenViewPluginDockWidgets) {
        _centralDockArea->setAllowedAreas(DockWidgetArea::CenterDockWidgetArea);
        _centralDockWidget.toggleView(true);
    }
    else {
        _centralDockArea->setAllowedAreas(DockWidgetArea::AllDockAreas);
        _centralDockWidget.toggleView(false);
    }
}
