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

#ifdef _DEBUG
#define LAYOUT_MANAGER_VERBOSE
#endif

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
    _initialized(false),
    _viewPluginDockWidgets(),
    _cachedDockWidgetsVisibility()
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
    _viewPluginDockWidgets.clear();
}

void LayoutManager::fromVariantMap(const QVariantMap& variantMap)
{
    _viewPluginDockWidgets.clear();

    variantMapMustContain(variantMap, "DockingManagers");

    const auto dockingManagersMap = variantMap["DockingManagers"].toMap();

    variantMapMustContain(dockingManagersMap, "Main");
    variantMapMustContain(dockingManagersMap, "ViewPlugins");
        
    _dockManager->fromVariantMap(dockingManagersMap["Main"].toMap());
    _viewPluginsDockWidget.getDockManager().fromVariantMap(dockingManagersMap["ViewPlugins"].toMap());

    qDebug() << _dockManager->getDockWidgetsOfType<ViewPluginDockWidget>().count();
    qDebug() << _viewPluginsDockWidget.getDockManager().getDockWidgetsOfType<ViewPluginDockWidget>().count();

    _viewPluginDockWidgets.append(_dockManager->getDockWidgetsOfType<ViewPluginDockWidget>());
    _viewPluginDockWidgets.append(_viewPluginsDockWidget.getDockManager().getDockWidgetsOfType<ViewPluginDockWidget>());
}

QVariantMap LayoutManager::toVariantMap() const
{
    const auto mainDockingManager           = _dockManager->toVariantMap();
    const auto viewPluginsDockingManager    = _viewPluginsDockWidget.getDockManager().toVariantMap();

    const QVariantMap dockingManagers = {
        { "Main", mainDockingManager },
        { "ViewPlugins", viewPluginsDockingManager }
    };

    return {
        { "DockingManagers", dockingManagers }
    };
}

void LayoutManager::addViewPlugin(plugin::ViewPlugin* viewPlugin, plugin::ViewPlugin* dockToViewPlugin /*= nullptr*/, DockAreaFlag dockArea /*= DockAreaFlag::Right*/)
{
    auto viewPluginDockWidget = new ViewPluginDockWidget(viewPlugin->getGuiName(), viewPlugin);

    _viewPluginDockWidgets << viewPluginDockWidget;

    if (viewPlugin->isSystemViewPlugin())
        _dockManager->addDockWidget(static_cast<DockWidgetArea>(dockArea), viewPluginDockWidget, _dockManager->findDockAreaWidget(&dockToViewPlugin->getWidget()));
    else
        _viewPluginsDockWidget.addViewPlugin(viewPluginDockWidget, dockToViewPlugin, dockArea);
}

void LayoutManager::isolateViewPlugin(plugin::ViewPlugin* viewPlugin, bool isolate)
{
#ifdef LAYOUT_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << viewPlugin->getGuiName() << isolate;
#endif

    if (isolate) {
        for (auto viewPluginDockWidget : _viewPluginDockWidgets) {
            if (viewPlugin == viewPluginDockWidget->getViewPlugin())
                continue;

            _cachedDockWidgetsVisibility[viewPluginDockWidget] = !viewPluginDockWidget->isClosed();

            viewPluginDockWidget->toggleView(false);
        }
    }
    else {
        for (auto dockWidget : _cachedDockWidgetsVisibility.keys())
            dockWidget->toggleView(_cachedDockWidgetsVisibility[dockWidget]);

        _cachedDockWidgetsVisibility.clear();
    }
}

}
