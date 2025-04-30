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

#include <string>
#include <unordered_set>
#include <vector>

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
    else if (_selectedDatasets.count() == 0 && !_allDatasets.isEmpty()) {
        addSeparator();
        addAction(getSelectionGroupPatternAction());
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

                const std::int32_t selectionGroupIndex = selectionIndexDialog.getSelectionGroupIndex();
                for (auto& selectedDataset : _selectedDatasets) {
                    selectedDataset->setGroupIndex(selectionGroupIndex);
                }

                mv::data().getSelectionGroupingAction()->setChecked(true);
            }

        }
        );

    return selectionGroupAction;
}

QAction* DataHierarchyWidgetContextMenu::getSelectionGroupPatternAction()
{
    auto selectionGroupPatternAction = new QAction("Selection group pattern...");

    selectionGroupPatternAction->setToolTip("Define a suffix for selection groups");
    selectionGroupPatternAction->setIcon(StyledIcon("ellipsis"));

    connect(selectionGroupPatternAction, &QAction::triggered,
        [this]() -> void {
            SelectionPatternGroupIndexDialog selectionPatternDialog(nullptr);
            selectionPatternDialog.setModal(true);
            const int ok = selectionPatternDialog.exec();

            if ((ok == QDialog::Accepted)) {

                const std::string selectionGroupPattern = selectionPatternDialog.getSelectionGroupPattern().toStdString();
                const std::int32_t selectionGroupOption = selectionPatternDialog.getSelectionGroupOption();
                
                if (selectionGroupPattern.length() == 0)
                    return;

                Datasets allDatasets = mv::data().getAllDatasets();

                if (allDatasets.isEmpty())
                    return;

                auto extractBase = [&selectionGroupOption, &selectionGroupPattern](const Datasets& allDatasets) -> std::unordered_set<std::string> {
                    std::unordered_set<std::string> potential_bases;
                    if (selectionGroupOption == 0) { // Suffix
                        // Identify potential base strings
                        for (const Dataset<DatasetImpl>& dataset : allDatasets) {
                            std::string datasetName = dataset->getGuiName().toStdString();
                            if (datasetName.ends_with(selectionGroupPattern)) {
                                size_t prefix_length = datasetName.length() - selectionGroupPattern.length();
                                potential_bases.insert(datasetName.substr(0, prefix_length));
                            }
                        }
                    }
                    else { // Prefix
                        potential_bases = { selectionGroupPattern };
                    }

                    return potential_bases;
                    };

                std::unordered_set<std::string> potential_bases = extractBase(allDatasets);

                // Do not continue if no matches were found
                if (potential_bases.empty())
                    return;

                for (const std::string_view potential_base : potential_bases)
                    qDebug() << potential_base;

                std::vector<std::vector<Dataset<DatasetImpl>>> selectionGroups;

                // Find masks and match them to bases
                for (const std::string_view potential_base : potential_bases) {
                    auto& current_results = selectionGroups.emplace_back(std::vector<Dataset<DatasetImpl>>{});
                    for (const Dataset<DatasetImpl>& dataset : allDatasets) {
                        std::string datasetName = dataset->getGuiName().toStdString();
                        if (datasetName.starts_with(potential_base)) {
                            current_results.push_back(dataset);
                        }
                    }
                }

                // Do not continue if no matches were found
                if (selectionGroups.empty() || std::all_of(selectionGroups.begin(), selectionGroups.end(), [](const std::vector<Dataset<DatasetImpl>>& v) { return v.empty();}))
                    return;

                std::int32_t selectionGroupStartIndex = selectionPatternDialog.getSelectionGroupIndex();

                for (std::vector<Dataset<DatasetImpl>>& selectionGroup : selectionGroups) {
                    for (auto& dataset : selectionGroup) {
                        dataset->setGroupIndex(selectionGroupStartIndex);
                    }
                    selectionGroupStartIndex++;
                }

                mv::data().getSelectionGroupingAction()->setChecked(true);
            }

        }
        );

    return selectionGroupPatternAction;
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
    _confirmAction(this, "Ok"),
    _selectionIndexAction(this, "Set the same selection group index for all selected datasets", -1, 250, -1)
{
    setWindowTitle(tr("Selection group index"));
    setWindowIcon(StyledIcon("ellipsis"));
    
    _confirmAction.setEnabled(false);
    _confirmAction.setToolTip("Selection group index must be larger than -1");

    connect(&_confirmAction, &TriggerAction::triggered, this, &SelectionGroupIndexDialog::closeDialogAction);
    connect(this, &SelectionGroupIndexDialog::closeDialog, this, &QDialog::accept);

    connect(&_selectionIndexAction, &IntegralAction::valueChanged, [this](int value) {
        _confirmAction.setEnabled(value >= 0);
        });

    auto groupAction = new HorizontalGroupAction(this, "Settings");

    groupAction->addAction(&_selectionIndexAction);
    groupAction->addAction(&_confirmAction);

    auto layout = new QHBoxLayout();

    layout->addWidget(groupAction->createWidget(this));

    setLayout(layout);
}

SelectionPatternGroupIndexDialog::SelectionPatternGroupIndexDialog(QWidget* parent) :
    QDialog(parent),
    _confirmAction(this, "Ok"),
    _selectionPatternAction(this, "Pattern"),
    _selectionOptionAction(this, "Setting"),
    _infoTextAction(this, "Info"),
    _selectionIndexAction(this, "Start selection group indices at", -1, 1024, -1)
{
    setWindowTitle(tr("Selection group pattern"));
    setWindowIcon(StyledIcon("ellipsis"));
    
    QString infoString = QStringLiteral("\"Prefix\" defines the prefix to match names with.\n"
        "\"Suffix\" is used to define a prefix, taken from entries with matching suffix.\n"
        "E.g.given the data names \"A\", \"A.end\", \"B\", \"B.end\" and a suffix \".end\",\n"
        "this will group (\"A\", \"A.end\") and (\"B\", \"B.end\")");

    _infoTextAction.setString(infoString);

    _confirmAction.setEnabled(false);
    _confirmAction.setToolTip("Selection group index must be larger than -1");

    _selectionOptionAction.initialize({ "Suffix", "Prefix" }, "Suffix");

    connect(&_confirmAction, &TriggerAction::triggered, this, &SelectionPatternGroupIndexDialog::closeDialogAction);
    connect(this, &SelectionPatternGroupIndexDialog::closeDialog, this, &QDialog::accept);

    connect(&_selectionIndexAction, &IntegralAction::valueChanged, [this](int value) {
        _confirmAction.setEnabled(value >= 0);
        });

    auto settingsGroupAction = new VerticalGroupAction(this, "Settings");

    settingsGroupAction->addAction(&_selectionPatternAction);
    settingsGroupAction->addAction(&_selectionOptionAction);
    settingsGroupAction->addAction(&_infoTextAction, StringAction::WidgetFlag::Label);
    settingsGroupAction->addAction(&_selectionIndexAction);
    settingsGroupAction->addAction(&_confirmAction);

    auto layout = new QVBoxLayout();

    layout->addWidget(settingsGroupAction->createWidget(this));

	setLayout(layout);
}
