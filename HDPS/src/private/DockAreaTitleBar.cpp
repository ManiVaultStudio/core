#include "DockAreaTitleBar.h"
#include "DockManager.h"
#include "ViewMenu.h"
#include "LoadSystemViewMenu.h"

#include <Application.h>
#include <CoreInterface.h>

#include <QToolButton>

using namespace ads;
using namespace hdps;
using namespace hdps::util;
using namespace hdps::gui;

DockAreaTitleBar::DockAreaTitleBar(ads::CDockAreaWidget* dockAreaWidget) :
    CDockAreaTitleBar(dockAreaWidget)
{
    auto addViewPluginToolButton = new QToolButton(dockAreaWidget);

    addViewPluginToolButton->setToolTip(QObject::tr("Add views"));
    addViewPluginToolButton->setIcon(Application::getIconFont("FontAwesome").getIcon("plus"));
    addViewPluginToolButton->setAutoRaise(true);
    addViewPluginToolButton->setPopupMode(QToolButton::InstantPopup);
    addViewPluginToolButton->setStyleSheet("QToolButton::menu-indicator { image: none; }");
    addViewPluginToolButton->setIconSize(QSize(8, 8));

    auto dockManager = dockAreaWidget->dockManager();

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

    const auto updateReadOnly = [addViewPluginToolButton]() -> void {
        addViewPluginToolButton->setVisible(!workspaces().getLockingAction().isLocked());
    };

    connect(&workspaces().getLockingAction().getLockedAction(), &ToggleAction::toggled, this, updateReadOnly);

    updateReadOnly();
}
