#include "VisualizationWidget.h"

#include <QVBoxLayout>

#include "DockAreaWidget.h"

using namespace ads;

VisualizationWidget::VisualizationWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _dockManager(new CDockManager(this)),
    _centralDockArea(nullptr),
    _centralDockWidget(""),
    _logoWidget(this)
{
    _centralDockWidget.setWidget(&_logoWidget);

    _centralDockArea = _dockManager.setCentralWidget(&_centralDockWidget);

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(&_dockManager);

    setLayout(layout);

    //connect(_dockManager.get(), &CDockManager::dockAreasAdded, this, &VisualizationWidget::updateCentralWidget);
    //connect(_dockManager.get(), &CDockManager::dockAreasRemoved, this, &VisualizationWidget::updateCentralWidget);
}

ads::CDockManager& VisualizationWidget::getDockManager()
{
    return _dockManager;
}

void VisualizationWidget::updateCentralWidget()
{
    if (_centralDockArea->openDockWidgetsCount() == 0) {
        _centralDockArea->setAllowedAreas(DockWidgetArea::CenterDockWidgetArea);

        //if (_centralDockWidget.isClosed())
        _centralDockWidget.toggleView(true);
    }
    else {
        _centralDockArea->setAllowedAreas(DockWidgetArea::AllDockAreas);
        _centralDockWidget.toggleView(false);
    }
}
