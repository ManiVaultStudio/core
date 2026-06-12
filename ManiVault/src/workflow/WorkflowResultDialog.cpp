// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowResultDialog.h"

#include <QHeaderView>

namespace mv::workflow
{

WorkflowResultDialog::WorkflowResultDialog(const SharedWorkflowResult& workflowResult, SeverityLevels levels /*= allSeverityLevels*/, QWidget* parent) :
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

    setStyleSheet(R"(
		QToolTip {
		    border: none;
		    background-color: palette(ToolTipBase);
		    color: palette(ToolTipText);
		    padding: 6px;
		}
	)");

    _messagesListModel.setWorkflowResult(workflowResult);
    _messagesFilterModel.setSourceModel(&_messagesListModel);

    auto& filterlevelAction = _messagesFilterModel.getFilterLevelAction();

    QStringList selectedOptions;

    for (const auto& level : levels)
        selectedOptions << getSeverityLevelName(level);

    filterlevelAction.setSelectedOptions(selectedOptions);

    _hierarchyWidget.getToolbarAction().addAction(&filterlevelAction);
    _hierarchyWidget.getFilterGroupAction().addAction(&filterlevelAction);

    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setSortingEnabled(true);
    treeView.sortByColumn(static_cast<int>(AbstractWorkflowMessagesModel::Column::TimeStamp), Qt::AscendingOrder);
    treeView.setRootIsDecorated(false);
    
    auto header = treeView.header();

    header->setStretchLastSection(false);

    header->setSectionResizeMode(static_cast<int>(AbstractWorkflowMessagesModel::Column::Level), QHeaderView::Fixed);
    header->setSectionResizeMode(static_cast<int>(AbstractWorkflowMessagesModel::Column::Emitter), QHeaderView::Interactive);
    header->setSectionResizeMode(static_cast<int>(AbstractWorkflowMessagesModel::Column::Location), QHeaderView::Interactive);
    header->setSectionResizeMode(static_cast<int>(AbstractWorkflowMessagesModel::Column::Text), QHeaderView::Stretch);
    header->setSectionResizeMode(static_cast<int>(AbstractWorkflowMessagesModel::Column::Details), QHeaderView::Interactive);

    header->resizeSection(static_cast<int>(AbstractWorkflowMessagesModel::Column::Level), 35);
    header->resizeSection(static_cast<int>(AbstractWorkflowMessagesModel::Column::Emitter), 150);
    header->resizeSection(static_cast<int>(AbstractWorkflowMessagesModel::Column::Location), 150);
    header->resizeSection(static_cast<int>(AbstractWorkflowMessagesModel::Column::Text), 300);
    header->resizeSection(static_cast<int>(AbstractWorkflowMessagesModel::Column::TimeStamp), 60);
    header->resizeSection(static_cast<int>(AbstractWorkflowMessagesModel::Column::Details), 100);
    
    //header->setSectionHidden(static_cast<int>(AbstractWorkflowMessagesModel::Column::Emitter), true);
    header->setSectionHidden(static_cast<int>(AbstractWorkflowMessagesModel::Column::TimeStamp), true);

    auto layout = new QVBoxLayout(this);

    layout->addWidget(&_hierarchyWidget);

    setLayout(layout);
}

QSize WorkflowResultDialog::sizeHint() const
{
	return {
        800,
        600
	};
}

}
