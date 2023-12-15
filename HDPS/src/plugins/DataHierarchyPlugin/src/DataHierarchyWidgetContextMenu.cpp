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

DataHierarchyWidgetContextMenu::DataHierarchyWidgetContextMenu(QWidget* parent, Datasets datasets) :
    QMenu(parent),
    _datasets(datasets)
{
    addMenusForPluginType(plugin::Type::ANALYSIS);
    addMenusForPluginType(plugin::Type::LOADER);
    addMenusForPluginType(plugin::Type::WRITER);
    addMenusForPluginType(plugin::Type::TRANSFORMATION);
    addMenusForPluginType(plugin::Type::VIEW);

    QSet<DataType> dataTypes;

    for (const auto& dataset : _datasets)
        dataTypes.insert(dataset->getDataType());

    if (datasets.count() >= 2 && dataTypes.count() == 1) {
        addSeparator();
        addAction(getGroupAction());
    }

    if (!Application::core()->requestAllDataSets().isEmpty()) {
        addSeparator();
        addMenu(getLockMenu());
        addMenu(getUnlockMenu());
    }

    addSeparator();
    
    auto removeDatasetsAction = new TriggerAction(this, QString("Remove dataset%1").arg(_datasets.count() >= 2 ? "s" : ""));

    removeDatasetsAction->setIconByName("trash");

    connect(removeDatasetsAction, &TriggerAction::triggered, this, [this]() -> void {
        Datasets datasetsToRemove;

        for (const auto& candidateDataset : _datasets)
            if (!candidateDataset->getDataHierarchyItem().isChildOf(_datasets))
                datasetsToRemove << candidateDataset;

        for (auto datasetToRemove : datasetsToRemove)
            qDebug() << "datasetToRemove" << datasetToRemove->getGuiName();
    });

    addAction(removeDatasetsAction);
}

void DataHierarchyWidgetContextMenu::addMenusForPluginType(plugin::Type pluginType)
{
    QMap<QString, QMenu*> menus;

    for (auto pluginTriggerAction : Application::core()->getPluginManager().getPluginTriggerActions(pluginType, _datasets)) {
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
    //menus["Analyze"]->setIcon(mv::plugin::getPluginTypeIcon(pluginType));
}

QAction* DataHierarchyWidgetContextMenu::getGroupAction()
{
    auto groupDataAction = new QAction("Group...");

    groupDataAction->setToolTip("Group datasets into one");
    groupDataAction->setIcon(Application::getIconFont("FontAwesome").getIcon("object-group"));

    connect(groupDataAction, &QAction::triggered, [this]() -> void {
        Application::core()->groupDatasets(_datasets);
    });

    return groupDataAction;
}

QMenu* DataHierarchyWidgetContextMenu::getLockMenu()
{
    auto lockMenu = new QMenu("Lock");

    lockMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("lock"));

    auto lockAllAction = new QAction("All");

    connect(lockAllAction, &QAction::triggered, this, [this]() -> void {
        for (auto dataset : Application::core()->requestAllDataSets())
            dataset->lock();
    });

    lockMenu->setEnabled(false);

    QVector<bool> locked;

    for (auto dataset : Application::core()->requestAllDataSets())
        locked << dataset->isLocked();

    const auto numberOfLockedDatasets = std::accumulate(locked.begin(), locked.end(), 0);

    lockMenu->setEnabled(numberOfLockedDatasets < Application::core()->requestAllDataSets().size());

    auto lockSelectedAction = new QAction("Selected");

    lockSelectedAction->setIcon(Application::getIconFont("FontAwesome").getIcon("mouse-pointer"));
    lockSelectedAction->setEnabled(!_datasets.isEmpty());

    connect(lockSelectedAction, &QAction::triggered, this, [this]() -> void {
        for (auto dataset : _datasets)
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

    QVector<bool> locked;

    for (auto dataset : Application::core()->requestAllDataSets())
        locked << dataset->isLocked();

    const auto numberOfLockedDatasets = std::accumulate(locked.begin(), locked.end(), 0);

    unlockMenu->setEnabled(numberOfLockedDatasets >= 1);

    auto unlockAllAction = new QAction("All");

    connect(unlockAllAction, &QAction::triggered, this, [this]() -> void {
        for (auto dataset : Application::core()->requestAllDataSets())
            dataset->unlock();
    });

    auto unlockSelectedAction = new QAction("Selected");

    unlockSelectedAction->setIcon(Application::getIconFont("FontAwesome").getIcon("mouse-pointer"));
    unlockSelectedAction->setEnabled(!_datasets.isEmpty());

    connect(unlockSelectedAction, &QAction::triggered, this, [this]() -> void {
        for (auto dataset : _datasets)
            dataset->unlock();
        });

    unlockMenu->addAction(unlockSelectedAction);
    unlockMenu->addAction(unlockAllAction);

    return unlockMenu;
}
