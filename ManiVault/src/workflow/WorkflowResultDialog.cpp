// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowResultDialog.h"
#include "WorkflowMessageDetailsDelegate.h"

#include "util/SeverityLevel.h"

#include <QHeaderView>
#include <QLabel>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>

using namespace mv::util;

namespace mv::workflow
{

WorkflowResultDialog::WorkflowResultDialog(const SharedWorkflowResult& workflowResult, SeverityLevels levels /*= allSeverityLevels*/, QWidget* parent) :
    QDialog(parent)
{
    Q_ASSERT(workflowResult);

    if (!workflowResult) {
        qDebug() << "WorkflowResultDialog: Invalid workflow result provided";
        return;
    }

    const auto& resultDetails = workflowResult->getOptions().reporting.resultDetails;

    const auto title    = resultDetails.makeTitle(workflowResult->getWorkflowName());
    const auto message  = resultDetails.makeMessage(workflowResult->getWorkflowName());

    setWindowTitle(title);
    setWindowIcon(StyledIcon("scroll"));

    auto layout = new QVBoxLayout(this);

    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(8);

    auto summaryLayout  = new QHBoxLayout();
    auto summaryIcon    = new QLabel(this);
    auto summaryLabel   = new QLabel(workflowResult->getSummaryText(), this);

    summaryIcon->setPixmap(workflowResult->getStatusIcon().pixmap(20, 20));

    summaryLabel->setWordWrap(true);
    summaryLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    summaryLabel->setStyleSheet(QStringLiteral("font-weight: bold;"));

    summaryLayout->addWidget(summaryIcon);
    summaryLayout->addWidget(summaryLabel, 1);

    layout->addLayout(summaryLayout);

    if (!message.isEmpty()) {
        auto messageLabel = new QLabel(message, this);

        messageLabel->setWordWrap(true);
        messageLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

        layout->addWidget(messageLabel);
    }

    _messagesListModel.setWorkflowResult(workflowResult);
    _messagesFilterModel.setSourceModel(&_messagesListModel);

    auto& filterlevelAction = _messagesFilterModel.getFilterLevelAction();

    filterlevelAction.setText(QStringLiteral("Severity"));

    auto toolbarAction = new gui::HorizontalGroupAction(this, "Messages", true);

    toolbarAction->addAction(&filterlevelAction);

    QStringList selectedOptions;

    for (const auto& level : levels)
        selectedOptions << getSeverityLevelName(level);

    filterlevelAction.setSelectedOptions(selectedOptions);

    auto treeView = new QTreeView(this);

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

    header->setSectionHidden(static_cast<int>(AbstractWorkflowMessagesModel::Column::Emitter), true);
    header->setSectionHidden(static_cast<int>(AbstractWorkflowMessagesModel::Column::Location), true);
    header->setSectionHidden(static_cast<int>(AbstractWorkflowMessagesModel::Column::TimeStamp), true);

    auto toggleButton = new QToolButton(this);

    toggleButton->setText(QStringLiteral("Diagnostics"));
    toggleButton->setCheckable(true);
    toggleButton->setChecked(workflowResult->hasWarnings() || workflowResult->hasErrors() || workflowResult->hasCriticalErrors());
    toggleButton->setArrowType(Qt::DownArrow);
    toggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toggleButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    toggleButton->setAutoRaise(false);
    toggleButton->setStyleSheet(QStringLiteral(
        "QToolButton {"
        "    text-align: left;"
        "    padding: 3px 3px;"
        "    border: 1px solid palette(mid);"
        "    border-radius: 3px;"
        "}"
    ));

    auto detailsWidget = new QWidget(this);
    auto detailsLayout = new QVBoxLayout(detailsWidget);

    detailsLayout->setContentsMargins(0, 0, 0, 0);
    detailsLayout->setSpacing(6);

    detailsLayout->addWidget(toolbarAction->createWidget(detailsWidget));
    detailsLayout->addWidget(treeView);

    layout->addWidget(toggleButton);
    layout->addWidget(detailsWidget);

    detailsWidget->setVisible(toggleButton->isChecked());

    connect(toggleButton, &QToolButton::toggled, this, [this, detailsWidget, toggleButton](bool expanded) {
        detailsWidget->setVisible(expanded);
        toggleButton->setArrowType(expanded ? Qt::DownArrow : Qt::RightArrow);
        adjustSize();
    });
}

QSize WorkflowResultDialog::sizeHint() const
{
    return {
        800,
        600
    };
}

}