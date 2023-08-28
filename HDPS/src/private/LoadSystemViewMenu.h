// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/DockArea.h>

#include <actions/TriggerAction.h>

#include <DockAreaWidget.h>

#include <QMenu>

/**
 * Load system menu class
 * 
 * Menu class for loading system view plugins
 * 
 * @author Thomas Kroes
 */
class LoadSystemViewMenu : public QMenu
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param dockAreaWidget Pointer to dock area widget to which new view plugins are docked (new view plugins will be docked top-level if nullptr)
     */
    LoadSystemViewMenu(QWidget *parent = nullptr, ads::CDockAreaWidget* dockAreaWidget = nullptr);

    /**
     * Get whether the menu has any enabled menu items that produce a plugin
     * return Boolean determining whether the menu has any enabled menu items that produce a plugin
     */
    bool mayProducePlugins() const;

    /**
     * Populate the menu
     */
    void populate();

private:

    /**
     * Get actions for loading system views
     * @param dockArea Dock area to dock to (if set to none, the view plugin preferred dock area is used)
     * @return Vector of actions
     */
    QVector<QPointer<hdps::gui::TriggerAction>> getLoadSystemViewsActions(hdps::gui::DockAreaFlag dockArea = hdps::gui::DockAreaFlag::None);

private:
    using MenuMap = QMap<hdps::gui::DockAreaFlag, QSharedPointer<QMenu>>;   /** Short hand for map to shared pointers of menus */

private:
    ads::CDockAreaWidget*   _dockAreaWidget;        /** Pointer to dock area widget to which new view plugins are docked (new view plugins will be docked top-level if nullptr) */
    MenuMap                 _loadViewsDockedMenus;  /** Map of shared pointer to menus */

};
