// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowResultDialog.h"

namespace mv::util
{

WorkflowResultDialog::WorkflowResultDialog(const SharedWorkflowResult& workflowResult, QWidget* parent) :
	QDialog(parent),   
	_hierarchyWidget(this, "Workflow message", _messagesListModel, &_messagesFilterModel)
{
    Q_ASSERT(workflowResult);

    if (!workflowResult) {
        qDebug() << "WorkflowResultDialog: Invalid workflow result provided";
        return;
    }

    setWindowTitle("Workflow result - " + workflowResult->getWorkflowName());
    setWindowIcon(StyledIcon("scroll"));

    _messagesListModel.setWorkflowResult(workflowResult);
    _messagesFilterModel.setSourceModel(&_messagesListModel);

    _hierarchyWidget.getToolbarAction().addAction(&_messagesFilterModel.getFilterLevelAction());
    _hierarchyWidget.getFilterGroupAction().addAction(&_messagesFilterModel.getFilterLevelAction());

    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setSortingEnabled(true);
    treeView.sortByColumn(static_cast<int>(AbstractWorkflowMessagesModel::Column::DateTime), Qt::AscendingOrder);

    auto layout = new QVBoxLayout(this);

    layout->addWidget(&_hierarchyWidget);

    setLayout(layout);
}

}
