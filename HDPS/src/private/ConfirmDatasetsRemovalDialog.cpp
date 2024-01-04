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
    _mainGroupAction(this, "Main group"),
    _messageAction(this, "Message"),
    _modusGroupAction(this, "Modus"),
    _configureModusGroupAction(this, "Select datasets to remove"),
    _selectDatasetsAction(this, "Select datasets"),
    _buttonsGroupAction(this, "Buttons group"),
    _removeAction(this, "Remove"),
    _cancelAction(this, "Cancel")
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("trash"));
    setWindowTitle("About to remove dataset(s)");
    setModal(true);

    _datasetsToRemoveModel.setDatasets(_selectedDatasets);

    _mainGroupAction.setShowLabels(false);

    _mainGroupAction.addAction(&_messageAction);
    _mainGroupAction.addAction(&_modusGroupAction);
    _mainGroupAction.addAction(&_buttonsGroupAction);

    _messageAction.setDefaultWidgetFlags(StringAction::WidgetFlag::Label);
    _messageAction.setString(QString("The selected dataset%1 contain%2 children, what would you like to do?").arg(_selectedDatasets.count() == 1 ? "" : "s", _selectedDatasets.count() == 1 ? "s" : ""));

    _modusGroupAction.setShowLabels(false);
    _modusGroupAction.addAction(&settings().getMiscellaneousSettings().getKeepDescendantsAfterRemovalAction());
    _modusGroupAction.addAction(&_configureModusGroupAction);

    _selectDatasetsAction.initialize(&_datasetsToRemoveModel, &_datasetsToRemoveFilterModel, "dataset");

    _configureModusGroupAction.setIconByName("check-square");
    _configureModusGroupAction.setShowLabels(false);
    _configureModusGroupAction.setPopupSizeHint(QSize(400, 400));
    _configureModusGroupAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

    _configureModusGroupAction.addAction(&_selectDatasetsAction, -1, [this](WidgetAction* action, QWidget* widget) -> void {
        auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget != nullptr);

        if (hierarchyWidget == nullptr)
            return;

        hierarchyWidget->setWindowIcon(Application::getIconFont("FontAwesome").getIcon("database"));

        auto treeView = widget->findChild<QTreeView*>("TreeView");

        Q_ASSERT(treeView != nullptr);

        if (treeView == nullptr)
            return;

        treeView->expandAll();
        treeView->setColumnHidden(static_cast<int>(DatasetsToRemoveModel::Column::DatasetId), true);
        treeView->setColumnHidden(static_cast<int>(DatasetsToRemoveModel::Column::Visible), true);

        auto treeViewHeader = treeView->header();

        treeViewHeader->setStretchLastSection(false);

        treeViewHeader->setSectionResizeMode(static_cast<int>(DatasetsToRemoveModel::Column::Name), QHeaderView::Stretch);
        treeViewHeader->setSectionResizeMode(static_cast<int>(DatasetsToRemoveModel::Column::DatasetId), QHeaderView::Stretch);

        /*
        const auto resizeSections = [this, treeViewHeader]() -> void {
            if (_datasetsToRemoveFilterModel.rowCount() >= 1)
                treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
        };

        resizeSections();

        connect(&_datasetsToRemoveFilterModel, &QAbstractItemModel::rowsInserted, this, resizeSections);
        connect(&_datasetsToRemoveFilterModel, &QAbstractItemModel::rowsRemoved, this, resizeSections);
        connect(&_datasetsToRemoveFilterModel, &QAbstractItemModel::layoutChanged, this, resizeSections);
        */
    });

    //_togglesGroupAction.addAction(&_datasetsToRemoveModel.getKeepChildrenAction());

    _removeAction.setToolTip("Remove the dataset(s)");
    _cancelAction.setToolTip("Do not remove the dataset(s) and quit this dialog");

    _buttonsGroupAction.addAction(&settings().getMiscellaneousSettings().getAskConfirmationBeforeRemovingDatasetsAction());
    _buttonsGroupAction.addStretch();
    _buttonsGroupAction.addAction(&_removeAction);
    _buttonsGroupAction.addAction(&_cancelAction);

    auto layout = new QVBoxLayout();

    setLayout(layout);

    layout->addWidget(_mainGroupAction.createWidget(this));
    //layout->addWidget(_buttonsGroupAction.createWidget(this));

    const auto updateRemoveAction = [this]() -> void {
        const auto numberOfDatasetsToRemove = getDatasetsToRemove().count();

        _removeAction.setEnabled(numberOfDatasetsToRemove >= 1);

        if (numberOfDatasetsToRemove == 0)
            _removeAction.setText("Remove");
        else
            _removeAction.setText(QString("Remove %1").arg(QString::number(numberOfDatasetsToRemove)));
    };

    updateRemoveAction();

    connect(&_datasetsToRemoveFilterModel, &QAbstractItemModel::rowsInserted, this, updateRemoveAction);
    connect(&_datasetsToRemoveFilterModel, &QAbstractItemModel::rowsRemoved, this, updateRemoveAction);
    connect(&_datasetsToRemoveFilterModel, &QAbstractItemModel::layoutChanged, this, updateRemoveAction);
    connect(&_datasetsToRemoveFilterModel, &QAbstractItemModel::dataChanged, this, updateRemoveAction);

    connect(&_removeAction, &TriggerAction::triggered, this, &ConfirmDatasetsRemovalDialog::accept);
    connect(&_cancelAction, &TriggerAction::triggered, this, &ConfirmDatasetsRemovalDialog::reject);
}

mv::Datasets ConfirmDatasetsRemovalDialog::getDatasetsToRemove() const
{
    return _datasetsToRemoveModel.getDatasetsToRemove();
}
