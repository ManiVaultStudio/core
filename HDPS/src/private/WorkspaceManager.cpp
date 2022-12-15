#include "WorkspaceManager.h"
#include "ViewPluginDockWidget.h"
#include "ViewMenu.h"
#include "LoadSystemViewMenu.h"

#include <Application.h>

#include <util/Serialization.h>
#include <util/Icon.h>

#include <DockComponentsFactory.h>
#include <DockAreaTitleBar.h>
#include <DockAreaTabBar.h>

#include <QMainWindow>
#include <QToolButton>
#include <QPainter>

#ifdef _DEBUG
    #define WORKSPACE_MANAGER_VERBOSE
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
        addViewPluginToolButton->setIconSize(QSize(16, 16));

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

namespace hdps
{

WorkspaceManager::WorkspaceManager() :
    AbstractWorkspaceManager(),
    _dockManager(),
    _viewPluginsWidget(),
    _initialized(false),
    _cachedDockWidgetsVisibility(),
    _loadWorkspaceAction(this, "Load"),
    _saveWorkspaceAction(this, "Save"),
    _saveWorkspaceAsAction(this, "Save As..."),
    _recentWorkspacesMenu(),
    _icon(),
    _filePath()
{
    setObjectName("WorkspaceManager");

    ads::CDockComponentsFactory::setFactory(new CustomComponentsFactory());

    _loadWorkspaceAction.setShortcut(QKeySequence("Ctrl+Shift+O"));
    _loadWorkspaceAction.setIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
    _loadWorkspaceAction.setToolTip("Open workspace from disk");

    _saveWorkspaceAction.setShortcut(QKeySequence("Ctrl+Shift+S"));
    _saveWorkspaceAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _saveWorkspaceAction.setToolTip("Save workspace to disk");

    _saveWorkspaceAsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _saveWorkspaceAsAction.setToolTip("Save workspace under a new file to disk");

    _recentWorkspacesMenu.setTitle("Recent...");
    _recentWorkspacesMenu.setToolTip("Recently opened workspaces");
    _recentWorkspacesMenu.setIcon(Application::getIconFont("FontAwesome").getIcon("clock"));

    auto mainWindow = Application::topLevelWidgets().first();

    mainWindow->addAction(&_loadWorkspaceAction);
    mainWindow->addAction(&_saveWorkspaceAction);
    mainWindow->addAction(&_saveWorkspaceAsAction);

    connect(&_loadWorkspaceAction, &QAction::triggered, [this](bool) {
        loadWorkspace();
    });

    connect(&_saveWorkspaceAction, &QAction::triggered, [this](bool) {
        saveWorkspace(_filePath);
    });

    connect(&_saveWorkspaceAsAction, &QAction::triggered, [this](bool) {
        saveWorkspaceAs();
    });

    createIcon();
    updateRecentWorkspacesMenu();
}

WorkspaceManager::~WorkspaceManager()
{
    reset();
} 

void WorkspaceManager::initalize()
{
#ifdef WORKSPACE_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif
}

void WorkspaceManager::reset()
{
#ifdef WORKSPACE_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
    }
    endReset();
}

void WorkspaceManager::initialize(QWidget* widget)
{
    if (_initialized)
        return;

    _dockManager        = new DockManager(widget);
    _viewPluginsWidget  = new ViewPluginsDockWidget(_dockManager);

    _dockManager->setObjectName("MainDockManager");

    auto dw = new CDockWidget("View plugins central dock widget");

    
    dw->setWidget(_viewPluginsWidget);

    auto viewPluginsDockArea = _dockManager->setCentralWidget(dw);

    //viewPluginsDockArea->setAllowedAreas(DockWidgetArea::NoDockWidgetArea);

    connect(&Application::core()->getPluginManager(), &AbstractPluginManager::pluginAboutToBeDestroyed, this, [this](plugin::Plugin* plugin) -> void {
        const auto viewPlugin = dynamic_cast<ViewPlugin*>(plugin);

        if (!viewPlugin)
            return;

        if (viewPlugin->isSystemViewPlugin())
            _dockManager->removeViewPluginDockWidget(viewPlugin);
        else
            _viewPluginsWidget->getDockManager().removeViewPluginDockWidget(viewPlugin);
    });

    _initialized = true;
}

void WorkspaceManager::loadWorkspace(QString filePath /*= ""*/)
{
    try
    {
#ifdef WORKSPACE_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << filePath;
#endif

        emit workspaceAboutToBeLoaded(filePath);
        {
            _filePath = filePath;
        }
        emit workspaceLoaded(filePath);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to load workspace", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to load workspace");
    }
}

void WorkspaceManager::saveWorkspace(QString filePath /*= ""*/)
{
    try
    {
#ifdef WORKSPACE_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << filePath;
#endif
        _filePath = filePath;

        addRecentWorkspace(_filePath);

        emit workspaceSaved(filePath);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to save workspace", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to save workspace");
    }
}

void WorkspaceManager::saveWorkspaceAs()
{
#ifdef WORKSPACE_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    saveWorkspace();
}

void WorkspaceManager::fromVariantMap(const QVariantMap& variantMap)
{
    variantMapMustContain(variantMap, "DockingManagers");

    const auto dockingManagersMap = variantMap["DockingManagers"].toMap();

    variantMapMustContain(dockingManagersMap, "Main");
    variantMapMustContain(dockingManagersMap, "ViewPlugins");
        
    _dockManager->fromVariantMap(dockingManagersMap["Main"].toMap());
    //_viewPluginsDockWidget.getDockManager().fromVariantMap(dockingManagersMap["ViewPlugins"].toMap());
}

QVariantMap WorkspaceManager::toVariantMap() const
{
    const auto mainDockingManager           = _dockManager->toVariantMap();
    //const auto viewPluginsDockingManager    = _viewPluginsDockWidget.getDockManager().toVariantMap();

    const QVariantMap dockingManagers = {
        { "Main", mainDockingManager },
        //{ "ViewPlugins", viewPluginsDockingManager }
    };

    return {
        { "DockingManagers", dockingManagers }
    };
}

void WorkspaceManager::addViewPlugin(plugin::ViewPlugin* viewPlugin, plugin::ViewPlugin* dockToViewPlugin /*= nullptr*/, DockAreaFlag dockArea /*= DockAreaFlag::Right*/)
{
    auto viewPluginDockWidget = new ViewPluginDockWidget(viewPlugin->getGuiName(), viewPlugin);

    if (viewPlugin->isSystemViewPlugin())
        _dockManager->addDockWidget(static_cast<DockWidgetArea>(dockArea), viewPluginDockWidget, _dockManager->findDockAreaWidget(dockToViewPlugin ? &dockToViewPlugin->getWidget() : nullptr));
    else
        _viewPluginsWidget->addViewPlugin(viewPluginDockWidget, dockToViewPlugin, dockArea);
}

void WorkspaceManager::isolateViewPlugin(plugin::ViewPlugin* viewPlugin, bool isolate)
{
#ifdef WORKSPACE_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << viewPlugin->getGuiName() << isolate;
#endif

    if (viewPlugin->isSystemViewPlugin())
        return;

    if (isolate) {
        for (auto viewPluginDockWidget : getViewPluginDockWidgets()) {
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

ViewPluginDockWidgets WorkspaceManager::getViewPluginDockWidgets()
{
    return _dockManager->getViewPluginDockWidgets();// << _viewPluginsDockWidget.getDockManager().getViewPluginDockWidgets();
}

QMenu* WorkspaceManager::getMenu(QWidget* parent /*= nullptr*/)
{
    auto menu = new QMenu("Workspace", parent);

    menu->setTitle("Workspace");
    menu->setIcon(_icon);
    menu->setToolTip("Workspace operations");
    menu->setEnabled(Application::core()->getProjectManager().hasProject());

    menu->addAction(&_loadWorkspaceAction);
    menu->addAction(&_saveWorkspaceAction);
    menu->addAction(&_saveWorkspaceAsAction);
    menu->addSeparator();
    menu->addMenu(&_recentWorkspacesMenu);

    return menu;
}

void WorkspaceManager::createIcon()
{
    const auto size             = 128;
    const auto halfSize         = size / 2;
    const auto margin           = 12;
    const auto spacing          = 14;
    const auto halfSpacing      = spacing / 2;
    const auto lineThickness    = 7.0;

    QPixmap pixmap(size, size);

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    painter.setWindow(0, 0, size, size);

    const auto drawWindow = [&](QRectF rectangle) -> void {
        painter.setBrush(Qt::black);
        painter.setPen(Qt::NoPen);
        painter.drawRect(rectangle);
    };

    drawWindow(QRectF(QPointF(margin, margin), QPointF(halfSize - halfSpacing, size - margin)));
    drawWindow(QRectF(QPointF(halfSize + halfSpacing, margin), QPointF(size - margin, halfSize - halfSpacing)));
    drawWindow(QRectF(QPointF(halfSize + halfSpacing, halfSize + halfSpacing), QPointF(size - margin, size - margin)));

    _icon = hdps::gui::createIcon(pixmap);
}

void WorkspaceManager::updateRecentWorkspacesMenu()
{
    _recentWorkspacesMenu.clear();

    const auto recentWorkspaces = Application::current()->getSetting("Workspaces/Recent", QVariantList()).toList();

    _recentWorkspacesMenu.setEnabled(!recentWorkspaces.isEmpty());

    for (const auto& recentWorkspace : recentWorkspaces) {
        const auto recentWorkspaceFilePath = recentWorkspace.toMap()["FilePath"].toString();

        if (!QFileInfo(recentWorkspaceFilePath).exists())
            continue;

        auto recentProjectAction = new QAction(recentWorkspaceFilePath);

        recentProjectAction->setIcon(_icon);
        recentProjectAction->setToolTip("Load " + recentWorkspaceFilePath + "(last opened on " + recentWorkspace.toMap()["DateTime"].toDate().toString() + ")");

        connect(recentProjectAction, &QAction::triggered, this, [recentWorkspaceFilePath]() -> void {
            Application::core()->getWorkspaceManager().loadWorkspace(recentWorkspaceFilePath);
        });

        _recentWorkspacesMenu.addAction(recentProjectAction);
    }
}

void WorkspaceManager::addRecentWorkspace(const QString& filePath)
{
    auto recentWorkspaces = Application::current()->getSetting("Workspaces/Recent", QVariantList()).toList();

    QVariantMap recentWorkspace {
        { "FilePath", filePath },
        { "DateTime", QDateTime::currentDateTime() }
    };

    for (auto recentWorkspace : recentWorkspaces)
        if (recentWorkspace.toMap()["FilePath"].toString() == filePath)
            recentWorkspaces.removeOne(recentWorkspace);

    recentWorkspaces.insert(0, recentWorkspace);

    Application::current()->setSetting("Workspaces/Recent", recentWorkspaces);
}

}
