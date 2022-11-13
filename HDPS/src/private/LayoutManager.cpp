#include "LayoutManager.h"

#include "util/Serialization.h"

#include <DockComponentsFactory.h>
#include <DockAreaTitleBar.h>
#include <DockAreaTabBar.h>

#include <QMainWindow>
#include <QToolButton>

using namespace ads;

using namespace hdps;
using namespace hdps::plugin;
using namespace hdps::util;

class CCustomComponentsFactory : public ads::CDockComponentsFactory
{
public:
    using Super = ads::CDockComponentsFactory;
    CDockAreaTitleBar* createDockAreaTitleBar(ads::CDockAreaWidget* DockArea) const override
    {
        auto TitleBar = new ads::CDockAreaTitleBar(DockArea);
        auto addViewPluginToolButton = new QToolButton(DockArea);

        addViewPluginToolButton->setToolTip(QObject::tr("Help"));
        addViewPluginToolButton->setIcon(Application::getIconFont("FontAwesome").getIcon("plus"));
        addViewPluginToolButton->setAutoRaise(true);
        
        int Index = TitleBar->indexOf(TitleBar->button(ads::TitleBarButtonTabsMenu));
        TitleBar->insertWidget(Index - 1, addViewPluginToolButton);
        return TitleBar;
    }

    //CDockAreaTabBar* createDockAreaTabBar(CDockAreaWidget* DockArea) const override {
    //    auto dockAreaTabBar = new ads::CDockAreaTabBar(DockArea);

    //    auto CustomButton = new QToolButton(DockArea);
    //    CustomButton->setToolTip(QObject::tr("Help"));

    //    
    //    return dockAreaTabBar;
    //}
};

namespace hdps::gui
{

LayoutManager::LayoutManager() :
    AbstractLayoutManager(),
    _dockManager(),
    _visualizationDockArea(nullptr),
    _visualizationDockWidget()
{
    setText("Layout manager");
    setObjectName("Layout");

    ads::CDockComponentsFactory::setFactory(new CCustomComponentsFactory());
}

LayoutManager::~LayoutManager()
{
}

void LayoutManager::initialize(QMainWindow* mainWindow)
{
    _dockManager = QSharedPointer<DockManager>::create(mainWindow);

    _visualizationDockArea = _dockManager->setCentralWidget(&_visualizationDockWidget);
    
    _visualizationDockArea->setAllowedAreas(DockWidgetArea::NoDockWidgetArea);
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
    auto viewPluginDockWidget = new CDockWidget(viewPlugin->getGuiName());

    viewPluginDockWidget->setIcon(viewPlugin->getIcon());

    connect(&viewPlugin->getWidget(), &QWidget::windowTitleChanged, this, [this, viewPluginDockWidget](const QString& title) {
        viewPluginDockWidget->setWindowTitle(title);
    });

    connect(&viewPlugin->getMayCloseAction(), &ToggleAction::toggled, this, [this, viewPluginDockWidget](bool toggled) {
        viewPluginDockWidget->setFeature(CDockWidget::DockWidgetClosable, toggled);
    });

    connect(&viewPlugin->getMayFloatAction(), &ToggleAction::toggled, this, [this, viewPluginDockWidget](bool toggled) {
        viewPluginDockWidget->setFeature(CDockWidget::DockWidgetFloatable, toggled);
    });

    connect(&viewPlugin->getMayMoveAction(), &ToggleAction::toggled, this, [this, viewPluginDockWidget](bool toggled) {
        viewPluginDockWidget->setFeature(CDockWidget::DockWidgetMovable, toggled);
    });

    connect(&viewPlugin->getAllowedDockingAreasAction(), &OptionsAction::selectedOptionsChanged, this, [this, viewPluginDockWidget](const QStringList& selectedOptions) {
        viewPluginDockWidget->dockAreaWidget()->setAllowedAreas(static_cast<DockWidgetAreas>(ViewPlugin::getDockWidgetAreas(selectedOptions)));
    });

    const auto connectToViewPluginVisibleAction = [this, viewPlugin](CDockWidget* dockWidget) -> void {
        connect(&viewPlugin->getVisibleAction(), &ToggleAction::toggled, this, [this, dockWidget](bool toggled) {
            dockWidget->toggleView(toggled);
        });
    };

    const auto disconnectFromViewPluginVisibleAction = [this, viewPlugin](CDockWidget* dockWidget) -> void {
        disconnect(&viewPlugin->getVisibleAction(), &ToggleAction::toggled, this, nullptr);
    };

    QObject::connect(viewPluginDockWidget, &CDockWidget::closed, this, [this, viewPlugin, viewPluginDockWidget, connectToViewPluginVisibleAction, disconnectFromViewPluginVisibleAction]() {
        disconnectFromViewPluginVisibleAction(viewPluginDockWidget);
        {
            viewPlugin->getVisibleAction().setChecked(false);
        }
        connectToViewPluginVisibleAction(viewPluginDockWidget);
    });

    connectToViewPluginVisibleAction(viewPluginDockWidget);

    if (viewPlugin->isStandardView())
        _dockManager->addDockWidget(RightDockWidgetArea, viewPluginDockWidget);
    else
        _visualizationDockWidget.addViewPlugin(viewPluginDockWidget);
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
        { "DockAreas", dockAreas },
    });
}

void LayoutManager::reset()
{
    _dockManager.reset();
}

}
