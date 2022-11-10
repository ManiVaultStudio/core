#include "LayoutManager.h"

#include <QMainWindow>

using namespace ads;
using namespace hdps::plugin;

namespace hdps::gui
{

LayoutManager::LayoutManager() :
    AbstractLayoutManager(),
    _dockManager(),
    _centralDockArea(nullptr),
    _lastDockAreaWidget(nullptr),
    _leftDockAreaWidget(nullptr),
    _rightDockAreaWidget(nullptr),
    _leftDockWidget("Left"),
    _centralDockWidget("Views"),
    _rightDockWidget("Right"),
    _startPageDockWidget("Start page")
    //_startPageWidget(nullptr),
{
    setText("Layout manager");
    setObjectName("Layout");
}

LayoutManager::~LayoutManager()
{
}

void LayoutManager::initialize(QMainWindow* mainWindow)
{
    _dockManager = QSharedPointer<ads::CDockManager>::create(mainWindow);

    _centralDockArea = _dockManager->setCentralWidget(&_centralDockWidget);
    
    _centralDockArea->setAllowedAreas(DockWidgetArea::AllDockAreas);

    //_leftDockAreaWidget = _dockManager->addDockWidget(LeftDockWidgetArea, &_leftDockWidget, _centralDockArea);
    //_rightDockAreaWidget = _dockManager->addDockWidget(RightDockWidgetArea, &_rightDockWidget, _centralDockArea);
    

    //_dockManager->topLevelDockArea()->setAllowedAreas(DockWidgetAreas::LeftDockWidgetArea | DockWidgetAreas::RightDockWidgetArea);

    //_dockManager->setCentralWidget(&_centralDockWidget);

    //_centralDockArea = _dockManager->addDockWidget(DockWidgetArea::CenterDockWidgetArea, &_startPageDockWidget);
}

void LayoutManager::fromVariantMap(const QVariantMap& variantMap)
{
}

QVariantMap LayoutManager::toVariantMap() const
{
    return dockContainerWidgetToVariant(_dockManager.get());
}

void LayoutManager::addViewPlugin(ViewPlugin* viewPlugin)
{
    auto dockWidget = new CDockWidget(viewPlugin->getGuiName());

    dockWidget->setIcon(viewPlugin->getIcon());

    //dockWidget->setWidget(&viewPlugin->getWidget(), CDockWidget::ForceNoScrollArea);

    //connect(&viewPlugin->getWidget(), &QWidget::windowTitleChanged, this, [this, dockWidget](const QString& title) {
    //    dockWidget->setWindowTitle(title);
    //    });

    //connect(&viewPlugin->getMayCloseAction(), &ToggleAction::toggled, this, [this, dockWidget](bool toggled) {
    //    dockWidget->setFeature(CDockWidget::DockWidgetClosable, toggled);
    //    });

    //connect(&viewPlugin->getMayFloatAction(), &ToggleAction::toggled, this, [this, dockWidget](bool toggled) {
    //    dockWidget->setFeature(CDockWidget::DockWidgetFloatable, toggled);
    //    });

    //connect(&viewPlugin->getMayMoveAction(), &ToggleAction::toggled, this, [this, dockWidget](bool toggled) {
    //    dockWidget->setFeature(CDockWidget::DockWidgetMovable, toggled);
    //    });

    //connect(&viewPlugin->getAllowedDockingAreasAction(), &OptionsAction::selectedOptionsChanged, this, [this, dockWidget](const QStringList& selectedOptions) {
    //    dockWidget->dockAreaWidget()->setAllowedAreas(static_cast<DockWidgetAreas>(ViewPlugin::getDockWidgetAreas(selectedOptions)));
    //    });

    //const auto connectToViewPluginVisibleAction = [this, viewPlugin](CDockWidget* dockWidget) -> void {
    //    connect(&viewPlugin->getVisibleAction(), &ToggleAction::toggled, this, [this, dockWidget](bool toggled) {
    //        dockWidget->toggleView(toggled);
    //        });
    //};

    //const auto disconnectFromViewPluginVisibleAction = [this, viewPlugin](CDockWidget* dockWidget) -> void {
    //    disconnect(&viewPlugin->getVisibleAction(), &ToggleAction::toggled, this, nullptr);
    //};

    //QObject::connect(dockWidget, &CDockWidget::closed, this, [this, viewPlugin, dockWidget, connectToViewPluginVisibleAction, disconnectFromViewPluginVisibleAction]() {
    //    disconnectFromViewPluginVisibleAction(dockWidget);
    //    {
    //        viewPlugin->getVisibleAction().setChecked(false);
    //    }
    //    connectToViewPluginVisibleAction(dockWidget);
    //});

    auto dockWidgetArea = RightDockWidgetArea;

    const auto preferredDockWidgetArea = viewPlugin->getPreferredDockingAreaAction().getCurrentText();

    //if (ViewPlugin::dockWidgetAreaMap.contains(preferredDockWidgetArea))
    //    dockWidgetArea = static_cast<DockWidgetArea>(ViewPlugin::dockWidgetAreaMap[preferredDockWidgetArea]);
    
    //if (viewPlugin->getCentralDockingAction().isChecked())
    _dockManager->addDockWidget(dockWidgetArea, dockWidget, _rightDockAreaWidget);
    //else
    //    _dockManager->addDockWidget(dockWidgetArea, dockWidget);

    //if (getViewPluginDockWidgets().isEmpty())
    //    dockWidgetArea = CenterDockWidgetArea;

    //if (_lastDockAreaWidget == nullptr)
    //    _lastDockAreaWidget = _dockManager->addDockWidget(dockWidgetArea, dockWidget);
    //else
    //    _lastDockAreaWidget = _dockManager->addDockWidget(dockWidgetArea, dockWidget, _lastDockAreaWidget);

    //QObject::connect(dockWidget->dockAreaWidget(), &CDockAreaWidget::currentChanged, [this](int index) {
    //    updateCentralWidget();
    //    });

    //QObject::connect(dockWidget, &CDockWidget::topLevelChanged, [this, dockWidget](bool topLevel) {
    //    updateCentralWidget();
    //    });

    //updateCentralWidget();
}

QVariantMap LayoutManager::dockWidgetToVariant(ads::CDockWidget* dockWidget) const
{
    Q_ASSERT(dockWidget != nullptr);

    auto dockWidgetVariant = QVariantMap();

    dockWidgetVariant["Title"] = dockWidget->windowTitle();

    return dockWidgetVariant;
}

QVariantMap LayoutManager::dockAreaWidgetToVariant(CDockAreaWidget* dockAreaWidget) const
{
    Q_ASSERT(dockAreaWidget != nullptr);

    auto dockAreaWidgetMap = QVariantMap();

    dockAreaWidgetMap["DockArea"] = "DockArea";

    auto dockContainer = dockAreaWidget->dockContainer();

    auto dockWidgetsList = QVariantList();

    for (std::int32_t dockWidgetIndex = 0; dockWidgetIndex < dockAreaWidget->dockWidgetsCount(); ++dockWidgetIndex)
        dockWidgetsList.push_back(dockWidgetToVariant(dockAreaWidget->dockWidget(dockWidgetIndex)));

    dockAreaWidgetMap["DockWidgets"] = dockWidgetsList;

    QVariantList splitterSizes;

    for (auto splitterSize : _dockManager->splitterSizes(dockAreaWidget))
        splitterSizes.push_back(splitterSize);

    dockAreaWidgetMap["SplitterSizes"] = splitterSizes;

    return dockAreaWidgetMap;
}

QVariantMap LayoutManager::dockContainerWidgetToVariant(CDockContainerWidget* dockContainerWidget) const
{
    Q_ASSERT(dockContainerWidget != nullptr);

    QVariantList dockAreas;

    for (std::int32_t dockAreaIndex = 0; dockAreaIndex < _dockManager->dockAreaCount(); ++dockAreaIndex)
        dockAreas.push_back(dockAreaWidgetToVariant(_dockManager->dockArea(dockAreaIndex)));

    return QVariantMap({
        { "DockAreas", dockAreas }
        });
}



}

//void MainWindow::initializeDocking()
//{
//    CDockManager::setConfigFlag(CDockManager::DragPreviewIsDynamic, true);
//    CDockManager::setConfigFlag(CDockManager::DragPreviewShowsContentPixmap, true);
//    CDockManager::setConfigFlag(CDockManager::DragPreviewShowsContentPixmap, true);
//    CDockManager::setConfigFlag(CDockManager::DragPreviewHasWindowFrame, true);
//    CDockManager::setConfigFlag(CDockManager::OpaqueSplitterResize, false);
//    CDockManager::setConfigFlag(CDockManager::FocusHighlighting, true);
//    CDockManager::setConfigFlag(CDockManager::EqualSplitOnInsertion, true);
//    CDockManager::setConfigFlag(CDockManager::DockAreaHasUndockButton, true);
//
//    initializeCentralDockingArea();
//
//    connect(_dockManager, &CDockManager::dockAreasAdded, this, &MainWindow::updateCentralWidget);
//    connect(_dockManager, &CDockManager::dockAreasRemoved, this, &MainWindow::updateCentralWidget);
//}
//void MainWindow::initializeCentralDockingArea()
//{
//    _startPageDockWidget->setIcon(Application::getIconFont("FontAwesome").getIcon("door-open"));
//    _startPageDockWidget->setWidget(_startPageWidget, CDockWidget::ForceNoScrollArea);
//
//    _centralDockArea = _dockManager->addDockWidget(DockWidgetArea::CenterDockWidgetArea, _startPageDockWidget);
//
//    _startPageDockWidget->setFeature(CDockWidget::DockWidgetClosable, false);
//    _startPageDockWidget->setFeature(CDockWidget::DockWidgetFloatable, false);
//    _startPageDockWidget->setFeature(CDockWidget::DockWidgetMovable, false);
//}
//
//void MainWindow::updateCentralWidget()
//{
//#ifdef MAIN_WINDOW_VERBOSE
//    qDebug().noquote() << "Update central widget with" << getViewPluginDockWidgets().count() << "view plugins";
//#endif
//
//    if (getViewPluginDockWidgets().count() == 0) {
//
//        // The only valid docking area is in the center when there is only one view plugin loaded
//        _centralDockArea->setAllowedAreas(DockWidgetArea::CenterDockWidgetArea);
//
//        _centralDockArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//
//        // Show the start page dock widget and set the mode
//        _startPageDockWidget->toggleView(true);
//        _startPageWidget->setMode(_core.requestAllDataSets().size() == 0 ? StartPageWidget::Mode::ProjectBar : StartPageWidget::Mode::LogoOnly);
//    }
//    else {
//
//        // Docking may occur anywhere
//        _centralDockArea->setAllowedAreas(DockWidgetArea::AllDockAreas);
//
//        // Hide the start page dock widget
//        _startPageDockWidget->toggleView(false);
//    }
//}
//
//QList<ads::CDockWidget*> MainWindow::getViewPluginDockWidgets(const bool& openOnly /*= true*/)
//{
//    QList<ads::CDockWidget*> viewPluginDockWidgets;
//
//    for (auto viewPluginDockWidget : _dockManager->dockWidgetsMap().values()) {
//        if (viewPluginDockWidget->property("PluginType").toString() != "View")
//            continue;
//
//        if (openOnly) {
//            if (!viewPluginDockWidget->isClosed() && !viewPluginDockWidget->isFloating())
//                viewPluginDockWidgets << viewPluginDockWidget;
//        }
//        else {
//            viewPluginDockWidgets << viewPluginDockWidget;
//        }
//    }
//
//    return viewPluginDockWidgets;
//}

//void MainWindow::addDockWidget(QWidget* widget, const QString& windowTitle, const DockWidgetArea& dockWidgetArea, CDockAreaWidget* dockAreaWidget)
//{
//    auto dockWidget = new CDockWidget(windowTitle);
//
//    _dockManager->addDockWidget(dockWidgetArea, dockWidget, dockAreaWidget);
//}