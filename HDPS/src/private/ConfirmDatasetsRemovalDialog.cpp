// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ConfirmDatasetsRemovalDialog.h"

#include <CoreInterface.h>
#include <Application.h>
#include <Set.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>

using namespace mv;

ConfirmDatasetsRemovalDialog::ConfirmDatasetsRemovalDialog(mv::Datasets selectedDatasets, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _selectedDatasets(selectedDatasets),
    _datasetsToRemoveModel(),
    _datasetsToRemoveFilterModel(),
    _datasetsHierarchyWidget(this, "Dataset", _datasetsToRemoveModel, &_datasetsToRemoveFilterModel),
    _togglesGroupAction(this, "Toggles"),
    _removeAction(this, "Remove"),
    _cancelAction(this, "Cancel"),
    _buttonsGroupAction(this, "Buttons group"),
    _settingsGroupAction(this, "Settings"),
    _treeAction(this, "Select datasets")
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("trash"));
    setWindowTitle("About to remove dataset(s)");
    setModal(true);

    connect(&_datasetsToRemoveModel.getAdvancedAction(), &ToggleAction::toggled, this, [this]() -> void {
        _datasetsHierarchyWidget.getTreeView().expandAll();
        _datasetsToRemoveFilterModel.invalidate();
    });

    const auto resizeSections = [this]() -> void {
        qDebug() << "resizeSections";

        if (_datasetsToRemoveFilterModel.rowCount() <= 0)
            return;

        _datasetsHierarchyWidget.getTreeView().header()->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
    };

    connect(&_datasetsToRemoveFilterModel, &QAbstractItemModel::rowsInserted, this, resizeSections);
    connect(&_datasetsToRemoveFilterModel, &QAbstractItemModel::rowsRemoved, this, resizeSections);
    connect(&_datasetsToRemoveFilterModel, &QAbstractItemModel::layoutChanged, this, resizeSections);

    _datasetsHierarchyWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("database"));

    _treeAction.initialize(&_datasetsToRemoveModel, &_datasetsToRemoveFilterModel, "dataset");

    _settingsGroupAction.setIconByName("cog");
    _settingsGroupAction.setShowLabels(false);
    _settingsGroupAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

    _settingsGroupAction.addAction(&_treeAction, -1, [](WidgetAction* action, QWidget* widget) -> void {
        auto treeView = widget->findChild<QTreeView*>("TreeView");

        Q_ASSERT(treeView != nullptr);

        if (treeView == nullptr)
            return;

        treeView->setColumnHidden(static_cast<int>(DatasetsToRemoveModel::Column::DatasetId), true);
        treeView->setColumnHidden(static_cast<int>(DatasetsToRemoveModel::Column::Visible), true);

        auto treeViewHeader = treeView->header();

        treeViewHeader->setStretchLastSection(false);

        treeViewHeader->setSectionResizeMode(static_cast<int>(DatasetsToRemoveModel::Column::Name), QHeaderView::Stretch);
        treeViewHeader->setSectionResizeMode(static_cast<int>(DatasetsToRemoveModel::Column::DatasetId), QHeaderView::Stretch);
    });

    _togglesGroupAction.addAction(&_datasetsToRemoveModel.getKeepChildrenAction());
    _togglesGroupAction.addAction(&_datasetsToRemoveModel.getAdvancedAction());
    _togglesGroupAction.addAction(&_settingsGroupAction);

    _removeAction.setToolTip("Remove the dataset(s)");
    _cancelAction.setToolTip("Do not remove the dataset(s) and quit this dialog");

    _buttonsGroupAction.addAction(&settings().getMiscellaneousSettings().getAskConfirmationBeforeRemovingDatasetsAction());
    _buttonsGroupAction.addStretch();
    _buttonsGroupAction.addAction(&_removeAction);
    _buttonsGroupAction.addAction(&_cancelAction);

    auto layout = new QVBoxLayout();

    setLayout(layout);

    layout->addWidget(_togglesGroupAction.createWidget(this));
    layout->addWidget(&_datasetsHierarchyWidget);
    layout->addWidget(_buttonsGroupAction.createWidget(this));

    connect(&_removeAction, &TriggerAction::triggered, this, &ConfirmDatasetsRemovalDialog::accept);
    connect(&_cancelAction, &TriggerAction::triggered, this, &ConfirmDatasetsRemovalDialog::reject);

    _datasetsToRemoveModel.setDatasets(_selectedDatasets);
}

mv::Datasets ConfirmDatasetsRemovalDialog::getDatasetsToRemove() const
{
    return _datasetsToRemoveModel.getDatasetsToRemove();
}
