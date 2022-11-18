#include "LayoutManager.h"
#include "ViewPluginDockWidget.h"
#include "ViewMenu.h"

#include <util/Serialization.h>

#include <DockComponentsFactory.h>
#include <DockAreaTitleBar.h>
#include <DockAreaTabBar.h>

#include <QMainWindow>
#include <QToolButton>

using namespace ads;

using namespace hdps;
using namespace hdps::plugin;
using namespace hdps::util;
using namespace hdps::gui;

class CCustomComponentsFactory : public ads::CDockComponentsFactory
{
public:
    using Super = ads::CDockComponentsFactory;

    CDockAreaTitleBar* createDockAreaTitleBar(ads::CDockAreaWidget* DockArea) const override
    {
        auto dockAreaTitleBar           = new ads::CDockAreaTitleBar(DockArea);
        auto addViewPluginToolButton    = new QToolButton(DockArea);

        addViewPluginToolButton->setToolTip(QObject::tr("Help"));
        addViewPluginToolButton->setIcon(Application::getIconFont("FontAwesome").getIcon("plus"));
        addViewPluginToolButton->setAutoRaise(true);
        addViewPluginToolButton->setPopupMode(QToolButton::InstantPopup);
        addViewPluginToolButton->setStyleSheet("QToolButton::menu-indicator { image: none; }");
        addViewPluginToolButton->setMenu(new ViewMenu(nullptr, ViewMenu::LoadViewPlugins));

        dockAreaTitleBar->insertWidget(dockAreaTitleBar->indexOf(dockAreaTitleBar->button(ads::TitleBarButtonTabsMenu)) - 1, addViewPluginToolButton);

        return dockAreaTitleBar;
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

void LayoutManager::reset()
{
    _dockManager.reset();
}

void LayoutManager::fromVariantMap(const QVariantMap& variantMap)
{
    _visualizationDockWidget.fromVariantMap(variantMap["Visualization"].toMap());
}

QVariantMap LayoutManager::toVariantMap() const
{
    return {
        { "Visualization", _visualizationDockWidget.toVariantMap() }
    };
}

void LayoutManager::addViewPlugin(plugin::ViewPlugin* viewPlugin)
{
    addViewPlugin(viewPlugin, nullptr, DockAreaFlag::Right);
}

void LayoutManager::addViewPlugin(plugin::ViewPlugin* viewPlugin, plugin::ViewPlugin* dockToViewPlugin, DockAreaFlag dockArea)
{
    auto viewPluginDockWidget = new ViewPluginDockWidget(viewPlugin->getGuiName(), viewPlugin);

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

    if (viewPlugin->isSystemViewPlugin())
        _dockManager->addDockWidget(RightDockWidgetArea, viewPluginDockWidget);
    else
        _visualizationDockWidget.addViewPlugin(viewPluginDockWidget, dockToViewPlugin, dockArea);
}

}
