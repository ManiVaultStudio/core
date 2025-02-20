// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "RemoveDatasetsDialog.h"

#include <CoreInterface.h>

#include <Set.h>

#include <widgets/HierarchyWidget.h>

#include <QVBoxLayout>
#include <QHeaderView>

using namespace mv;
using namespace mv::util;

RemoveDatasetsDialog::RemoveDatasetsDialog(mv::Datasets selectedDatasets, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _selectedDatasets(selectedDatasets),
    _topLevelDatasets(),
    _descendantDatasets(),
    _candidateDatasets(),
    _model(),
    _filterModel(),
    _mainGroupAction(this, "Main group"),
    _messageAction(this, "Message"),
    _descendantsModeAction(this, "Descendants", { "Keep", "Remove" }),
    _selectDatasetsGroupAction(this, "Select datasets to remove"),
    _selectDatasetsAction(this, "Select datasets"),
    _selectAllDatasetsAction(this, "Select all"),
    _bottomHorizontalGroupAction(this, "Bottom group"),
    _showAgainAction(this, "Show again", true),
    _removeAction(this, "Remove"),
    _cancelAction(this, "Cancel")
{
    setWindowIcon(StyledIcon("trash"));
    setWindowTitle("About to remove dataset(s)");
    setModal(true);

    _model.setDatasets(_selectedDatasets);

    _mainGroupAction.setShowLabels(false);

    _mainGroupAction.addAction(&_messageAction);
    _mainGroupAction.addAction(&_descendantsModeAction);
    _mainGroupAction.addStretch();
    _mainGroupAction.addAction(&_bottomHorizontalGroupAction);

    DataHierarchyItems selectedDataHierarchyItems, topLevelSelectedDataHierarchyItems;

    for (auto& selectedDataset : selectedDatasets)
        selectedDataHierarchyItems << &selectedDataset->getDataHierarchyItem();

    for (auto& topLevelDatasetCandidate : selectedDatasets)
        if (!topLevelDatasetCandidate->getDataHierarchyItem().isChildOf(selectedDataHierarchyItems))
            _topLevelDatasets << topLevelDatasetCandidate;

    for (const auto& topLevelDatasetToRemove : _topLevelDatasets)
        for (auto descendantDataHierarchyItem : topLevelDatasetToRemove->getDataHierarchyItem().getChildren(true))
            _descendantDatasets << descendantDataHierarchyItem->getDataset();

    _candidateDatasets << _topLevelDatasets << _descendantDatasets;

    _messageAction.setDefaultWidgetFlags(StringAction::WidgetFlag::Label);
    _messageAction.setString(QString("The selected dataset%1 contain%2 %3 child%4, what would you like to do with them?").arg(_selectedDatasets.count() == 1 ? "" : "s", _selectedDatasets.count() == 1 ? "s" : "", QString::number(_descendantDatasets.count()), _descendantDatasets.count() >= 2 ? "ren" : ""));

    _descendantsModeAction.setDefaultWidgetFlags(OptionAction::WidgetFlag::HorizontalButtons);
    _descendantsModeAction.setCurrentIndex(settings().getMiscellaneousSettings().getKeepDescendantsAfterRemovalAction().isChecked() ? 0 : 1);

    connect(&_descendantsModeAction, &OptionAction::currentIndexChanged, this, [this](const std::int32_t& currentIndex) -> void {
        settings().getMiscellaneousSettings().getKeepDescendantsAfterRemovalAction().setChecked(currentIndex == 0);
    });

    _selectDatasetsAction.initialize(&_model, &_filterModel, "dataset");

    _selectAllDatasetsAction.setToolTip("Select all datasets");

    connect(&_selectAllDatasetsAction, &TriggerAction::triggered, &_model, &DatasetsToRemoveModel::checkAll);

    _selectDatasetsGroupAction.setIconByName("mouse-pointer");
    _selectDatasetsGroupAction.setShowLabels(false);
    _selectDatasetsGroupAction.setPopupSizeHint(QSize(400, 400));
    _selectDatasetsGroupAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

    _selectDatasetsGroupAction.addAction(&_descendantsModeAction);
    _selectDatasetsGroupAction.addAction(&_selectDatasetsAction, -1, [this](WidgetAction* action, QWidget* widget) -> void {
        auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget != nullptr);

        if (hierarchyWidget == nullptr)
            return;

        hierarchyWidget->setWindowIcon(StyledIcon("database"));

        auto treeView = widget->findChild<QTreeView*>("TreeView");

        Q_ASSERT(treeView != nullptr);

        if (treeView == nullptr)
            return;

        treeView->expandAll();
        treeView->setColumnHidden(static_cast<int>(DatasetsToRemoveModel::Column::DatasetId), true);

        auto treeViewHeader = treeView->header();

        treeViewHeader->setStretchLastSection(false);

        treeViewHeader->setSectionResizeMode(static_cast<int>(DatasetsToRemoveModel::Column::Name), QHeaderView::Stretch);
        treeViewHeader->setSectionResizeMode(static_cast<int>(DatasetsToRemoveModel::Column::DatasetId), QHeaderView::Stretch);

        const auto resizeSections = [this, treeViewHeader]() -> void {
            if (_filterModel.rowCount() >= 1)
                treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
        };

        resizeSections();

        connect(&_filterModel, &QAbstractItemModel::rowsInserted, widget, resizeSections);
        connect(&_filterModel, &QAbstractItemModel::rowsRemoved, widget, resizeSections);
        connect(&_filterModel, &QAbstractItemModel::layoutChanged, widget, resizeSections);
    });

    _selectDatasetsGroupAction.addAction(&_selectAllDatasetsAction);

    _bottomHorizontalGroupAction.setShowLabels(false);

    _bottomHorizontalGroupAction.addAction(&_showAgainAction);
    _bottomHorizontalGroupAction.addStretch();
    _bottomHorizontalGroupAction.addAction(&_selectDatasetsGroupAction);
    _bottomHorizontalGroupAction.addAction(&_removeAction);
    _bottomHorizontalGroupAction.addAction(&_cancelAction);

    _showAgainAction.setChecked(settings().getMiscellaneousSettings().getAskConfirmationBeforeRemovingDatasetsAction().isChecked());

    connect(&_showAgainAction, &ToggleAction::toggled, this, [this](bool toggled) -> void {
        settings().getMiscellaneousSettings().getAskConfirmationBeforeRemovingDatasetsAction().setChecked(toggled);
    });

    _removeAction.setToolTip("Remove the dataset(s)");
    _cancelAction.setToolTip("Do not remove the dataset(s) and quit this dialog");

    _updateRemoveActionTimer.setSingleShot(true);
    _updateRemoveActionTimer.setInterval(10);

    const auto processModelChange = [this]() -> void {
        const auto numberOfDatasetsToRemove = getDatasetsToRemove().count();

        _removeAction.setEnabled(numberOfDatasetsToRemove >= 1);

        if (numberOfDatasetsToRemove == 0)
            _removeAction.setText("Remove");
        else
            _removeAction.setText(QString("Remove %1").arg(QString::number(numberOfDatasetsToRemove)));

        _selectAllDatasetsAction.setEnabled(numberOfDatasetsToRemove < _candidateDatasets.count());
    };

    const auto modelChanged = [this, processModelChange]() -> void {
        if (!_updateRemoveActionTimer.isActive()) {
            processModelChange();
            _updateRemoveActionTimer.start();
        }
        else {
            _updateRemoveActionTimer.start();
        }
    };

    connect(&_updateRemoveActionTimer, &QTimer::timeout, this, processModelChange);

    processModelChange();

    connect(&_filterModel, &QAbstractItemModel::rowsInserted, this, modelChanged);
    connect(&_filterModel, &QAbstractItemModel::rowsRemoved, this, modelChanged);
    connect(&_filterModel, &QAbstractItemModel::layoutChanged, this, modelChanged);
    connect(&_filterModel, &QAbstractItemModel::dataChanged, this, modelChanged);

    connect(&_descendantsModeAction, &OptionAction::currentIndexChanged, this, processModelChange);

    connect(&_removeAction, &TriggerAction::triggered, this, &RemoveDatasetsDialog::accept);
    connect(&_cancelAction, &TriggerAction::triggered, this, &RemoveDatasetsDialog::reject);

    auto layout = new QVBoxLayout();

    setLayout(layout);

    layout->addWidget(_mainGroupAction.createWidget(this));
}

mv::Datasets RemoveDatasetsDialog::getDatasetsToRemove() const
{
    return _model.getDatasetsToRemove();
}
