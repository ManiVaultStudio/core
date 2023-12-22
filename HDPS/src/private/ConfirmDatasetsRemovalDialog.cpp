// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ConfirmDatasetsRemovalDialog.h"

#include <CoreInterface.h>
#include <Application.h>
#include <Set.h>

#include <QVBoxLayout>
#include <QLabel>

using namespace mv;

ConfirmDatasetsRemovalDialog::ConfirmDatasetsRemovalDialog(mv::Datasets selectedDatasets, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _selectedDatasets(selectedDatasets),
    _datasetsToRemoveModel(),
    _datasetsToRemoveFilterModel(),
    _datasetsHierarchyWidget(this, "Dataset", _datasetsToRemoveModel, &_datasetsToRemoveFilterModel),
    _removeAction(this, "Remove"),
    _cancelAction(this, "Cancel"),
    _buttonsGroupAction(this, "Buttons group")
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("trash"));
    setWindowTitle("About to remove dataset(s)");
    setModal(true);

    _datasetsHierarchyWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("database"));

    _datasetsHierarchyWidget.getTreeView().setColumnHidden(static_cast<int>(DatasetsToRemoveModel::Column::DatasetId), true);

    _removeAction.setToolTip("Remove the dataset(s)");
    _cancelAction.setToolTip("Do not remove the dataset(s) and quit this dialog");

    _buttonsGroupAction.addAction(&settings().getMiscellaneousSettings().getConfirmDatasetsRemovalAction());
    _buttonsGroupAction.addStretch();
    _buttonsGroupAction.addAction(&_removeAction);
    _buttonsGroupAction.addAction(&_cancelAction);

    auto layout = new QVBoxLayout();

    setLayout(layout);

    layout->addWidget(&_datasetsHierarchyWidget);
    layout->addWidget(_buttonsGroupAction.createWidget(this));

    connect(&_removeAction, &TriggerAction::triggered, this, &ConfirmDatasetsRemovalDialog::accept);
    connect(&_cancelAction, &TriggerAction::triggered, this, &ConfirmDatasetsRemovalDialog::reject);

    _datasetsToRemoveModel.setSelectedDatasets(_selectedDatasets);
}
