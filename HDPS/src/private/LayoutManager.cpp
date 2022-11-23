#include "LayoutManager.h"
#include "ViewPluginDockWidget.h"
#include "ViewMenu.h"
#include "LoadSystemViewMenu.h"

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

class DockAreaTitleBar : public CDockAreaTitleBar
{
public:
    DockAreaTitleBar(ads::CDockAreaWidget* dockAreaWidget) :
        CDockAreaTitleBar(dockAreaWidget)
    {
        auto addViewPluginToolButton = new QToolButton(dockAreaWidget);

        addViewPluginToolButton->setToolTip(QObject::tr("Add views"));
        addViewPluginToolButton->setIcon(Application::getIconFont("FontAwesome").getIcon("plus"));
        addViewPluginToolButton->setAutoRaise(true);
        addViewPluginToolButton->setPopupMode(QToolButton::InstantPopup);
        addViewPluginToolButton->setStyleSheet("QToolButton::menu-indicator { image: none; }");

        auto dockManager = dockAreaWidget->dockManager();

        QMenu* menu = nullptr;

        if (dockManager->objectName() == "MainDockManager") {
            auto loadSystemViewMenu = new LoadSystemViewMenu(nullptr, dockAreaWidget);

            const auto updateToolButtonReadOnly = [addViewPluginToolButton, loadSystemViewMenu]() -> void {
                addViewPluginToolButton->setEnabled(loadSystemViewMenu->mayProducePlugins());
            };

            connect(dockAreaWidget->dockManager(), &CDockManager::dockWidgetAdded, this, updateToolButtonReadOnly);
            connect(dockAreaWidget->dockManager(), &CDockManager::dockWidgetRemoved, this, updateToolButtonReadOnly);

            addViewPluginToolButton->setMenu(loadSystemViewMenu);

            updateToolButtonReadOnly();
        }

        if (dockManager->objectName() == "ViewPluginsDockManager") {
            auto loadViewMenu = new ViewMenu(nullptr, ViewMenu::LoadViewPlugins, dockAreaWidget);

            const auto updateToolButtonReadOnly = [addViewPluginToolButton, loadViewMenu]() -> void {
                addViewPluginToolButton->setEnabled(loadViewMenu->mayProducePlugins());
            };

            connect(dockAreaWidget->dockManager(), &CDockManager::dockWidgetAdded, this, updateToolButtonReadOnly);
            connect(dockAreaWidget->dockManager(), &CDockManager::dockWidgetRemoved, this, updateToolButtonReadOnly);

            addViewPluginToolButton->setMenu(loadViewMenu);

            updateToolButtonReadOnly();
        }

        insertWidget(indexOf(button(ads::TitleBarButtonTabsMenu)) - 1, addViewPluginToolButton);
    }
};

class CustomComponentsFactory : public ads::CDockComponentsFactory, public QObject
{
public:
    using Super = ads::CDockComponentsFactory;

    CDockAreaTitleBar* createDockAreaTitleBar(ads::CDockAreaWidget* dockAreaWidget) const override
    {
        return new DockAreaTitleBar(dockAreaWidget);
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
    _viewPluginsDockArea(nullptr),
    _viewPluginsDockWidget(),
    _initialized(false)
{
    setText("Layout manager");
    setObjectName("LayoutManager");

    ads::CDockComponentsFactory::setFactory(new CustomComponentsFactory());
}

LayoutManager::~LayoutManager()
{
}

void LayoutManager::initialize(QMainWindow* mainWindow)
{
    if (_initialized)
        return;

    _dockManager = QSharedPointer<DockManager>::create(mainWindow);

    _dockManager->setObjectName("MainDockManager");

    _viewPluginsDockArea = _dockManager->setCentralWidget(&_viewPluginsDockWidget);
    
    _viewPluginsDockArea->setAllowedAreas(DockWidgetArea::NoDockWidgetArea);

    _initialized = true;
}

void LayoutManager::reset()
{
    _dockManager.reset();
}

void LayoutManager::fromVariantMap(const QVariantMap& variantMap)
{
    variantMapMustContain(variantMap, "Docking");
    variantMapMustContain(variantMap, "Visualization");
        
    _dockManager->fromVariantMap(variantMap["Docking"].toMap());

    _viewPluginsDockWidget.fromVariantMap(variantMap["Visualization"].toMap());
}

QVariantMap LayoutManager::toVariantMap() const
{
    return {
        { "Docking", _dockManager->toVariantMap() },
        { "Visualization", _viewPluginsDockWidget.toVariantMap() }
    };
}

void LayoutManager::addViewPlugin(plugin::ViewPlugin* viewPlugin, plugin::ViewPlugin* dockToViewPlugin /*= nullptr*/, DockAreaFlag dockArea /*= DockAreaFlag::Right*/)
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
        _dockManager->addDockWidget(static_cast<DockWidgetArea>(dockArea), viewPluginDockWidget, _dockManager->findDockAreaWidget(&dockToViewPlugin->getWidget()));
    else
        _viewPluginsDockWidget.addViewPlugin(viewPluginDockWidget, dockToViewPlugin, dockArea);
}

}
