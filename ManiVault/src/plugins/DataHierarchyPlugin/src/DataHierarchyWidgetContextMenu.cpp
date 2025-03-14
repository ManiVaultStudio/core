// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyWidgetContextMenu.h"

#include <Application.h>
#include <CoreInterface.h>
#include <Dataset.h>
#include <Set.h>

#include <actions/PluginTriggerAction.h>

#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QClipboard>

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
        addAction(getSelectionGroupAction());
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

        const auto numberOfLockedDatasets = std::count_if(_selectedDatasets.begin(), _selectedDatasets.end(), [](auto selectedDataset) -> bool { return selectedDataset->isLocked(); });

        removeDatasetsAction->setEnabled(numberOfLockedDatasets == 0);
        removeDatasetsAction->setIconByName("trash");

        connect(removeDatasetsAction, &TriggerAction::triggered, this, [this]() -> void {
            mv::data().removeDatasets(_selectedDatasets);
        });

        addAction(removeDatasetsAction);
    }

    if (!_selectedDatasets.isEmpty()) {
        addSeparator();

        const auto copySelectedDatasetIdsToClipboard = [this]() -> void {
            QStringList datasetIds, datasetIdsLog;

            for (const auto& selectedDataset : _selectedDatasets) {
	            datasetIds << selectedDataset->getId();
            	datasetIdsLog << selectedDataset->getId(mv::settings().getMiscellaneousSettings().getShowSimplifiedGuidsAction().isChecked());
            }

            QGuiApplication::clipboard()->setText(datasetIds.join("\n"));

            if (datasetIdsLog.count() > 1)
				qDebug() << "Dataset identifiers" << datasetIdsLog.join(", ") << "copied to clipboard";
            else
                qDebug() << "Dataset identifier" << datasetIdsLog.join(", ") << "copied to clipboard";
        };

        auto copyAction = new QAction(QString("Copy dataset ID%1").arg(_selectedDatasets.count() > 1 ? "'s" : ""));

        copyAction->setIcon(StyledIcon("barcode"));

        connect(copyAction, &QAction::triggered, copyAction, copySelectedDatasetIdsToClipboard);

        addAction(copyAction);
    }
}

void DataHierarchyWidgetContextMenu::addMenusForPluginType(plugin::Type pluginType)
{
    QMap<QString, QMenu*> menus;

    for (const auto& pluginTriggerAction : Application::core()->getPluginManager().getPluginTriggerActions(pluginType, _selectedDatasets)) {
        const auto titleSegments = pluginTriggerAction->getMenuLocation().split("/");

        QString menuPath, previousMenuPath = titleSegments.first();

        for (const auto& titleSegment : titleSegments) {
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
                            //menus[menuPath]->setIcon(StyledIcon("object-group"));

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
    groupDataAction->setIcon(StyledIcon("object-group"));

    connect(groupDataAction, &QAction::triggered, [this]() -> void {
        mv::data().groupDatasets(_selectedDatasets);
    });

    return groupDataAction;
}

QAction* DataHierarchyWidgetContextMenu::getSelectionGroupAction()
{
    auto selectionGroupAction = new QAction("Selection group...");

    selectionGroupAction->setToolTip("Add datasets to the same selection group");
    selectionGroupAction->setIcon(StyledIcon("ellipsis"));

    connect(selectionGroupAction, &QAction::triggered, 
        [this]() -> void {
            SelectionGroupIndexDialog selectionIndexDialog(nullptr);
            selectionIndexDialog.setModal(true);
            const int ok = selectionIndexDialog.exec();

            if ((ok == QDialog::Accepted)) {

                const int SelectionGroupIndex = selectionIndexDialog.getSelectionGroupIndex();
                for (auto& selectedDataset : _selectedDatasets) {
                    selectedDataset->setGroupIndex(SelectionGroupIndex);
                }

                mv::data().getSelectionGroupingAction()->setChecked(true);
            }

        }
        );

    return selectionGroupAction;
}

QMenu* DataHierarchyWidgetContextMenu::getLockMenu()
{
    auto lockMenu = new QMenu("Lock");

    lockMenu->setIcon(StyledIcon("lock"));

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

    lockSelectedAction->setIcon(StyledIcon("arrow-pointer"));
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

    unlockMenu->setIcon(StyledIcon("unlock"));
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

    unlockSelectedAction->setIcon(StyledIcon("arrow-pointer"));
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

    hideMenu->setIcon(StyledIcon("eye-slash"));
    hideMenu->setEnabled(!_selectedDatasets.isEmpty());

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
            dataset->getDataHierarchyItem().setVisible(false);
    });

    hideAllAction->setEnabled(_selectedDatasets.isEmpty() && (numberOfVisibleItems >= 1));

    auto hideSelectedAction = new QAction("Selected");

    hideSelectedAction->setIcon(StyledIcon("arrow-pointer"));
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

    unhideMenu->setIcon(StyledIcon("eye"));
    
    const auto numberOfHiddenItems = std::count_if(_allDatasets.begin(), _allDatasets.end(), [](auto& dataset) -> bool {
        return !dataset->getDataHierarchyItem().isVisible();
    });

    const auto numberOfSelectedHiddenItems = std::count_if(_selectedDatasets.begin(), _selectedDatasets.end(), [](auto& dataset) -> bool {
        return !dataset->getDataHierarchyItem().isVisible();
    });

    unhideMenu->setEnabled(candidateDatasetsToUnhide.count() >= 1);

    auto unhideAllAction = new QAction("All");

    connect(unhideAllAction, &QAction::triggered, this, [candidateDatasetsToUnhide]() -> void {
        for (auto candidateDatasetToUnhide : candidateDatasetsToUnhide)
            candidateDatasetToUnhide->getDataHierarchyItem().setVisible(true);
    });

    unhideAllAction->setEnabled(numberOfHiddenItems >= 1);

    unhideMenu->addAction(unhideAllAction);

    if (!candidateDatasetsToUnhide.isEmpty()) {
        unhideMenu->addSeparator();

        for (auto datasetToUnhide : candidateDatasetsToUnhide) {
            auto dataHierarchyItem = &datasetToUnhide->getDataHierarchyItem();

            auto unhideDatasetAction = new QAction(dataHierarchyItem->getLocation(true));

            unhideDatasetAction->setIcon(StyledIcon(datasetToUnhide->icon()));

            connect(unhideDatasetAction, &QAction::triggered, this, [dataHierarchyItem]() -> void {
                dataHierarchyItem->setVisible(true);
            });

            unhideMenu->addAction(unhideDatasetAction);
        }
    }

    return unhideMenu;
}

SelectionGroupIndexDialog::SelectionGroupIndexDialog(QWidget* parent) :
    QDialog(parent),
    confirmButton(this, "Ok"),
    selectionIndexAction(this, "Selection group index", -1, 250, -1)
{
    setWindowTitle(tr("Selection group index"));

    QLabel* indicesLabel = new QLabel("Set the same selection group index for all selected datasets:");

    confirmButton.setEnabled(false);
    confirmButton.setToolTip("Selection group index must be larger than -1");

    connect(&confirmButton, &TriggerAction::triggered, this, &SelectionGroupIndexDialog::closeDialogAction);
    connect(this, &SelectionGroupIndexDialog::closeDialog, this, &QDialog::accept);

    connect(&selectionIndexAction, &IntegralAction::valueChanged, [this](int value) {
        confirmButton.setEnabled(value >= 0);
        });

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(indicesLabel);
    layout->addWidget(selectionIndexAction.createWidget(this));
    layout->addWidget(confirmButton.createWidget(this));
    setLayout(layout);
}
