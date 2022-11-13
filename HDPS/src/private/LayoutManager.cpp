#include "LayoutManager.h"

#include "util/Serialization.h"

#include <QMainWindow>

using namespace ads;
using namespace hdps::plugin;
using namespace hdps::util;

namespace hdps::gui
{

LayoutManager::LayoutManager() :
    AbstractLayoutManager(),
    _dockManager(),
    _centralDockArea(nullptr),
    _lastDockAreaWidget(nullptr),
    _centralDockWidget("Views"),
    _visualizationWidget()
{
    setText("Layout manager");
    setObjectName("Layout");
}

LayoutManager::~LayoutManager()
{
}

void LayoutManager::initialize(QMainWindow* mainWindow)
{
    _dockManager = QSharedPointer<DockManager>::create(mainWindow);

    _centralDockWidget.setWidget(&_visualizationWidget);

    _centralDockArea = _dockManager->setCentralWidget(&_centralDockWidget);
    
    _centralDockArea->setAllowedAreas(DockWidgetArea::NoDockWidgetArea);

    //_dockManager->addDockWidget(CenterDockWidgetArea, &_centralDockWidget);
    //_dockManager->topLevelDockArea()->setAllowedAreas(DockWidgetAreas::LeftDockWidgetArea | DockWidgetAreas::RightDockWidgetArea);

    connect(_dockManager.get(), &DockManager::dockAreasAdded, this, &LayoutManager::updateCentralWidget);
    connect(_dockManager.get(), &DockManager::dockAreasRemoved, this, &LayoutManager::updateCentralWidget);

    _dockManager->initialize();
}

void LayoutManager::fromVariantMap(const QVariantMap& variantMap)
{
    variantMapMustContain(variantMap, "DockManager");

    _dockManager->fromVariantMap(variantMap["DockManager"].toMap());
}

QVariantMap LayoutManager::toVariantMap() const
{
    return {
        { "DockManager", _dockManager->toVariantMap() }
    };
}

void LayoutManager::addViewPlugin(ViewPlugin* viewPlugin)
{
    auto dockWidget = new CDockWidget(viewPlugin->getGuiName());

    dockWidget->setIcon(viewPlugin->getIcon());

    //dockWidget->setWidget(&viewPlugin->getWidget(), CDockWidget::ForceNoScrollArea);

    connect(&viewPlugin->getWidget(), &QWidget::windowTitleChanged, this, [this, dockWidget](const QString& title) {
        dockWidget->setWindowTitle(title);
    });

    connect(&viewPlugin->getMayCloseAction(), &ToggleAction::toggled, this, [this, dockWidget](bool toggled) {
        dockWidget->setFeature(CDockWidget::DockWidgetClosable, toggled);
    });

    connect(&viewPlugin->getMayFloatAction(), &ToggleAction::toggled, this, [this, dockWidget](bool toggled) {
        dockWidget->setFeature(CDockWidget::DockWidgetFloatable, toggled);
    });

    connect(&viewPlugin->getMayMoveAction(), &ToggleAction::toggled, this, [this, dockWidget](bool toggled) {
        dockWidget->setFeature(CDockWidget::DockWidgetMovable, toggled);
    });

    connect(&viewPlugin->getAllowedDockingAreasAction(), &OptionsAction::selectedOptionsChanged, this, [this, dockWidget](const QStringList& selectedOptions) {
        dockWidget->dockAreaWidget()->setAllowedAreas(static_cast<DockWidgetAreas>(ViewPlugin::getDockWidgetAreas(selectedOptions)));
    });

    const auto connectToViewPluginVisibleAction = [this, viewPlugin](CDockWidget* dockWidget) -> void {
        connect(&viewPlugin->getVisibleAction(), &ToggleAction::toggled, this, [this, dockWidget](bool toggled) {
            qDebug() << toggled;
            dockWidget->toggleView(toggled);
        });
    };

    const auto disconnectFromViewPluginVisibleAction = [this, viewPlugin](CDockWidget* dockWidget) -> void {
        disconnect(&viewPlugin->getVisibleAction(), &ToggleAction::toggled, this, nullptr);
    };

    QObject::connect(dockWidget, &CDockWidget::closed, this, [this, viewPlugin, dockWidget, connectToViewPluginVisibleAction, disconnectFromViewPluginVisibleAction]() {
        disconnectFromViewPluginVisibleAction(dockWidget);
        {
            viewPlugin->getVisibleAction().setChecked(false);
        }
        connectToViewPluginVisibleAction(dockWidget);
    });

    if (viewPlugin->isStandardView()) {
        _dockManager->addDockWidget(RightDockWidgetArea, dockWidget);
    }
    else {
        auto& centralWidgetDockManager = _visualizationWidget.getDockManager();

        if (_lastDockAreaWidget == nullptr)
            _lastDockAreaWidget = centralWidgetDockManager.addDockWidget(CenterDockWidgetArea, dockWidget);
        else
            _lastDockAreaWidget = centralWidgetDockManager.addDockWidget(RightDockWidgetArea, dockWidget, _lastDockAreaWidget);
    }

    connect(dockWidget->dockAreaWidget(), &CDockAreaWidget::currentChanged, [this](int index) {
        updateCentralWidget();
    });

    connect(dockWidget, &CDockWidget::viewToggled, [this, dockWidget](bool toggled) {
        updateCentralWidget();
    });

    connect(dockWidget, &CDockWidget::topLevelChanged, [this, dockWidget](bool topLevel) {
        updateCentralWidget();
    });

    updateCentralWidget();
}

QVariantMap LayoutManager::dockWidgetToVariant(ads::CDockWidget* dockWidget) const
{
    Q_ASSERT(dockWidget != nullptr);

    auto dockWidgetVariant = QVariantMap();

    dockWidgetVariant["Title"] = dockWidget->windowTitle();

    qDebug() << dockWidget->dockContainer();

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
        { "DockAreas", dockAreas }//,
        //{ "State", QVariant::fromValue(_dockManager->saveState().toBase64()) }
    });
}

void LayoutManager::reset()
{
    _dockManager.reset();
}

void LayoutManager::updateCentralWidget()
{
    return;
    qDebug() << __FUNCTION__ << _centralDockArea->dockWidgetsCount();
    
    if (_centralDockArea->openDockWidgetsCount() == 0) {
        _centralDockArea->setAllowedAreas(DockWidgetArea::CenterDockWidgetArea);
        //_centralDockArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        //if (_centralDockWidget.isClosed())
        //_centralDockWidget.toggleView(true);
    }
    else {
        _centralDockArea->setAllowedAreas(DockWidgetArea::AllDockAreas);
        _centralDockWidget.toggleView(false);
    }

    /*
    qDebug() << _dockManager->dockAreaCount();
    for (int i = 0; i < _dockManager->dockAreaCount(); i++)
        _dockManager->dockArea(i)->setAllowedAreas(DockWidgetAreas(LeftDockWidgetArea | RightDockWidgetArea));
    */
}

}
