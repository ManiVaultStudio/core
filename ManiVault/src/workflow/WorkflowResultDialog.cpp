// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowResultDialog.h"
#include "WorkflowMessageDetailsDelegate.h"

#include "util/SeverityLevel.h"

#include <QHeaderView>

using namespace mv::util;

namespace mv::workflow
{

WorkflowResultDialog::WorkflowResultDialog(const SharedWorkflowResult& workflowResult, SeverityLevels levels /*= allSeverityLevels*/, OptionalWorkflowOptions options /*= {}*/, QWidget* parent) :
	QDialog(parent)
{
    Q_ASSERT(workflowResult);

    if (!workflowResult) {
        qDebug() << "WorkflowResultDialog: Invalid workflow result provided";
        return;
    }

    const auto resolvedOptions      = options.value_or(WorkflowOptions{});
    const auto resultDetailsTitle   = resolvedOptions.reporting.resultDetailsTitle;

    setWindowTitle(resultDetailsTitle.isEmpty() ? "Workflow result - " + workflowResult->getWorkflowName() : resultDetailsTitle);
    setWindowIcon(StyledIcon("scroll"));

 //   setStyleSheet(R"(
	//	QToolTip {
	//	    border: none;
	//	    background-color: palette(ToolTipBase);
	//	    color: palette(ToolTipText);
	//	    padding: 6px;
	//	}
	//)");

    _messagesListModel.setWorkflowResult(workflowResult);
    _messagesFilterModel.setSourceModel(&_messagesListModel);

    auto& filterlevelAction = _messagesFilterModel.getFilterLevelAction();

    auto toolbarAction = new gui::HorizontalGroupAction(this, "Options", true);

    toolbarAction->addAction(&filterlevelAction);

    QStringList selectedOptions;

    for (const auto& level : levels)
        selectedOptions << getSeverityLevelName(level);

    filterlevelAction.setSelectedOptions(selectedOptions);

    auto treeView = new QTreeView();

    treeView->setModel(&_messagesFilterModel);
    treeView->setSortingEnabled(true);
    treeView->sortByColumn(static_cast<int>(AbstractWorkflowMessagesModel::Column::TimeStamp), Qt::AscendingOrder);
    treeView->setRootIsDecorated(false);
    treeView->setItemDelegateForColumn(static_cast<int>(AbstractWorkflowMessagesModel::Column::Details), new WorkflowMessageDetailsDelegate(treeView));
    treeView->setMouseTracking(true);

    auto header = treeView->header();
    header->setStretchLastSection(false);

    header->setSectionResizeMode(static_cast<int>(AbstractWorkflowMessagesModel::Column::Level), QHeaderView::Fixed);
    header->setSectionResizeMode(static_cast<int>(AbstractWorkflowMessagesModel::Column::Emitter), QHeaderView::Interactive);
    header->setSectionResizeMode(static_cast<int>(AbstractWorkflowMessagesModel::Column::Location), QHeaderView::Interactive);
    header->setSectionResizeMode(static_cast<int>(AbstractWorkflowMessagesModel::Column::Text), QHeaderView::Stretch);
    header->setSectionResizeMode(static_cast<int>(AbstractWorkflowMessagesModel::Column::Details), QHeaderView::Fixed);

    header->resizeSection(static_cast<int>(AbstractWorkflowMessagesModel::Column::Level), 35);
    header->resizeSection(static_cast<int>(AbstractWorkflowMessagesModel::Column::Emitter), 150);
    header->resizeSection(static_cast<int>(AbstractWorkflowMessagesModel::Column::Location), 150);
    header->resizeSection(static_cast<int>(AbstractWorkflowMessagesModel::Column::Text), 300);
    header->resizeSection(static_cast<int>(AbstractWorkflowMessagesModel::Column::TimeStamp), 60);
    header->resizeSection(static_cast<int>(AbstractWorkflowMessagesModel::Column::Details), 60);
    
    header->setSectionHidden(static_cast<int>(AbstractWorkflowMessagesModel::Column::TimeStamp), true);

    auto layout = new QVBoxLayout(this);

    if (const auto& resultDetailsMessage = resolvedOptions.reporting.resultDetailsMessage; !resultDetailsMessage.isEmpty())
        layout->addWidget(new QLabel(resultDetailsMessage, this));

    layout->addWidget(toolbarAction->createWidget(this));
    layout->addWidget(treeView);

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
