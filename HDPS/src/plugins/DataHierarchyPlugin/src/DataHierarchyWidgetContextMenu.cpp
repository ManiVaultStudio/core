// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyWidgetContextMenu.h"

#include <Application.h>
#include <CoreInterface.h>
#include <Dataset.h>
#include <Set.h>

#include <actions/PluginTriggerAction.h>
#include <actions/TriggerAction.h>

#include <QDebug>
#include <QMenu>
#include <QAction>

#include <stdexcept>

using namespace mv;
using namespace mv::util;
using namespace mv::plugin;
using namespace mv::gui;

DataHierarchyWidgetContextMenu::DataHierarchyWidgetContextMenu(QWidget* parent, Datasets selectedDatasets) :
    QMenu(parent),
    _allDatasets(mv::data().getAllDatasets()),
    _selectedDatasets(selectedDatasets)
{
    addMenusForPluginType(plugin::Type::ANALYSIS);
    addMenusForPluginType(plugin::Type::LOADER);
    addMenusForPluginType(plugin::Type::WRITER);
    addMenusForPluginType(plugin::Type::TRANSFORMATION);
    addMenusForPluginType(plugin::Type::VIEW);

    QSet<DataType> dataTypes;

    for (const auto& selectedDataset : _selectedDatasets)
        dataTypes.insert(selectedDataset->getDataType());

    if (_selectedDatasets.count() >= 2 && dataTypes.count() == 1) {
        addSeparator();
        addAction(getGroupAction());
    }

    if (!_allDatasets.isEmpty()) {
        addSeparator();
        
        addMenu(getLockMenu());
        addMenu(getUnlockMenu());

        addSeparator();

        addMenu(getHideMenu());
        addMenu(getUnhideMenu());
    }

    if (!_selectedDatasets.isEmpty()) {
        addSeparator();

        auto removeDatasetsAction = new TriggerAction(this, QString("Remove dataset%1").arg(_selectedDatasets.count() >= 2 ? "s" : ""));

        removeDatasetsAction->setIconByName("trash");

        connect(removeDatasetsAction, &TriggerAction::triggered, this, [this]() -> void {
            mv::data().removeDatasets(_selectedDatasets);
        });

        addAction(removeDatasetsAction);
    }
}

void DataHierarchyWidgetContextMenu::addMenusForPluginType(plugin::Type pluginType)
{
    QMap<QString, QMenu*> menus;

    for (auto pluginTriggerAction : Application::core()->getPluginManager().getPluginTriggerActions(pluginType, _selectedDatasets)) {
        const auto titleSegments = pluginTriggerAction->getMenuLocation().split("/");

        QString menuPath, previousMenuPath = titleSegments.first();

        for (auto titleSegment : titleSegments) {
            if (titleSegment != titleSegments.first() && titleSegment != titleSegments.last())
                menuPath += "/";

            menuPath += titleSegment;

            if (!menus.contains(menuPath)) {
                menus[menuPath] = new QMenu(titleSegment);

                if (titleSegment != titleSegments.first()) {
                    if (titleSegment == titleSegments.last())
                        menus[previousMenuPath]->addAction(pluginTriggerAction);
                    else {
                        if (titleSegment == "Group") {
                            //menus[menuPath]->setIcon(Application::getIconFont("FontAwesome").getIcon("object-group"));

                            if (menus[previousMenuPath]->actions().isEmpty())
                                menus[previousMenuPath]->addMenu(menus[menuPath]);
                            else
                                menus[previousMenuPath]->insertMenu(menus[previousMenuPath]->actions().first(), menus[menuPath]);

                            if (menus[previousMenuPath]->actions().count() >= 2)
                                menus[previousMenuPath]->insertSeparator(menus[previousMenuPath]->actions()[1]);
                        } else
                            menus[previousMenuPath]->addMenu(menus[menuPath]);
                    }
                                
                } else
                    addMenu(menus[titleSegments.first()]);
            }

            previousMenuPath = menuPath;
        }
    }
            
    //menus["Analyze"]->setTitle(mv::plugin::getPluginTypeName(pluginType));
    //menus["Import"]->setIcon(mv::plugin::getPluginTypeIcon(pluginType));
}

QAction* DataHierarchyWidgetContextMenu::getGroupAction()
{
    auto groupDataAction = new QAction("Group...");

    groupDataAction->setToolTip("Group datasets into one");
    groupDataAction->setIcon(Application::getIconFont("FontAwesome").getIcon("object-group"));

    connect(groupDataAction, &QAction::triggered, [this]() -> void {
        Application::core()->groupDatasets(_selectedDatasets);
    });

    return groupDataAction;
}

QMenu* DataHierarchyWidgetContextMenu::getLockMenu()
{
    auto lockMenu = new QMenu("Lock");

    lockMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("lock"));

    auto lockAllAction = new QAction("All");

    connect(lockAllAction, &QAction::triggered, this, [this]() -> void {
        for (auto& dataset : _allDatasets)
            dataset->lock();
    });

    lockMenu->setEnabled(false);

    const auto numberOfLockedDatasets = std::count_if(_allDatasets.begin(), _allDatasets.end(), [](auto& dataset) -> bool {
        return dataset->isLocked();
    });

    lockMenu->setEnabled(numberOfLockedDatasets < _allDatasets.size());

    auto lockSelectedAction = new QAction("Selected");

    lockSelectedAction->setIcon(Application::getIconFont("FontAwesome").getIcon("mouse-pointer"));
    lockSelectedAction->setEnabled(!_selectedDatasets.isEmpty());

    connect(lockSelectedAction, &QAction::triggered, this, [this]() -> void {
        for (auto& dataset : _selectedDatasets)
            dataset->lock();
    });

    lockMenu->addAction(lockSelectedAction);
    lockMenu->addAction(lockAllAction);

    return lockMenu;
}

QMenu* DataHierarchyWidgetContextMenu::getUnlockMenu()
{
    auto unlockMenu = new QMenu("Unlock");

    unlockMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("unlock"));
    unlockMenu->setEnabled(false);

    const auto numberOfLockedDatasets = std::count_if(_allDatasets.begin(), _allDatasets.end(), [](auto& dataset) -> bool {
        return dataset->isLocked();
    });

    unlockMenu->setEnabled(numberOfLockedDatasets >= 1);

    auto unlockAllAction = new QAction("All");

    connect(unlockAllAction, &QAction::triggered, this, [this]() -> void {
        for (auto& dataset : _allDatasets)
            dataset->unlock();
    });

    auto unlockSelectedAction = new QAction("Selected");

    unlockSelectedAction->setIcon(Application::getIconFont("FontAwesome").getIcon("mouse-pointer"));
    unlockSelectedAction->setEnabled(!_selectedDatasets.isEmpty());

    connect(unlockSelectedAction, &QAction::triggered, this, [this]() -> void {
        for (auto& dataset : _selectedDatasets)
            dataset->unlock();
    });

    unlockMenu->addAction(unlockSelectedAction);
    unlockMenu->addAction(unlockAllAction);

    return unlockMenu;
}

QMenu* DataHierarchyWidgetContextMenu::getHideMenu()
{
    auto hideMenu = new QMenu("Hide");

    hideMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("eye-slash"));

    const auto numberOfVisibleItems = std::count_if(_allDatasets.begin(), _allDatasets.end(), [](auto& dataset) -> bool {
        return dataset->getDataHierarchyItem().isVisible();
    });

    const auto numberOfSelectedVisibleItems = std::count_if(_selectedDatasets.begin(), _selectedDatasets.end(), [](auto& dataset) -> bool {
        return dataset->getDataHierarchyItem().isVisible();
    });

    hideMenu->setEnabled(_selectedDatasets.empty() ? numberOfVisibleItems >= 1 : numberOfSelectedVisibleItems >= 1);

    auto hideAllAction = new QAction("All");

    connect(hideAllAction, &QAction::triggered, this, [this]() -> void {
        for (auto& dataset : _allDatasets)
            dataset->getDataHierarchyItem();
    });

    hideAllAction->setEnabled(numberOfVisibleItems < _allDatasets.size());

    auto hideSelectedAction = new QAction("Selected");

    hideSelectedAction->setIcon(Application::getIconFont("FontAwesome").getIcon("mouse-pointer"));
    hideSelectedAction->setEnabled(!_selectedDatasets.isEmpty());

    connect(hideSelectedAction, &QAction::triggered, this, [this]() -> void {
        for (auto& dataset : _selectedDatasets)
            dataset->getDataHierarchyItem().setVisible(false);
    });

    hideMenu->addAction(hideSelectedAction);
    hideMenu->addAction(hideAllAction);

    return hideMenu;
}

QMenu* DataHierarchyWidgetContextMenu::getUnhideMenu()
{
    Datasets candidateDatasetsToUnhide;

    if (_selectedDatasets.isEmpty()) {
        for (auto& dataset : _allDatasets)
            if (!dataset->getDataHierarchyItem().isVisible())
                candidateDatasetsToUnhide << dataset;
    }
    else {
        for (auto& selectedDataset : _selectedDatasets)
            for (auto child : selectedDataset->getChildren())
                if (!child->getDataHierarchyItem().isVisible())
                    candidateDatasetsToUnhide << child;
    }

    auto unhideMenu = new QMenu("Unhide");

    unhideMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("eye"));
    
    const auto numberOfHiddenItems = std::count_if(_allDatasets.begin(), _allDatasets.end(), [](auto& dataset) -> bool {
        return !dataset->getDataHierarchyItem().isVisible();
    });

    const auto numberOfSelectedHiddenItems = std::count_if(_selectedDatasets.begin(), _selectedDatasets.end(), [](auto& dataset) -> bool {
        return !dataset->getDataHierarchyItem().isVisible();
    });

    unhideMenu->setEnabled(candidateDatasetsToUnhide.count() >= 1);

    auto unhideAllAction = new QAction("All");

    connect(unhideAllAction, &QAction::triggered, this, [this]() -> void {
        for (auto& dataset : _allDatasets)
            dataset->getDataHierarchyItem().setVisible(true);
    });

    unhideAllAction->setEnabled(numberOfHiddenItems < _allDatasets.size());

    unhideMenu->addAction(unhideAllAction);

    if (!candidateDatasetsToUnhide.isEmpty()) {
        unhideMenu->addSeparator();

        for (auto datasetToUnhide : candidateDatasetsToUnhide) {
            auto dataHierarchyItem = &datasetToUnhide->getDataHierarchyItem();

            auto unhideDatasetAction = new QAction(dataHierarchyItem->getLocation(true));

            unhideDatasetAction->setIcon(datasetToUnhide->getIcon());

            connect(unhideDatasetAction, &QAction::triggered, this, [dataHierarchyItem]() -> void {
                dataHierarchyItem->setVisible(true);
            });

            unhideMenu->addAction(unhideDatasetAction);
        }
    }

    return unhideMenu;
}
