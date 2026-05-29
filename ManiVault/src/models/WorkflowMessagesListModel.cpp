// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowMessagesListModel.h"

#include <QDebug>

#ifdef _DEBUG
    #define WORKFLOW_MESSAGES_LIST_MODEL_VERBOSE
#endif

namespace mv {

WorkflowMessagesListModel::WorkflowMessagesListModel(QObject* parent /*= nullptr*/) :
    AbstractWorkflowMessagesModel(parent)
{
}

void WorkflowMessagesListModel::setWorkflowResult(const util::SharedWorkflowResult& workflowResult)
{
    for (const auto& message : workflowResult->getMessages()) {
        appendRow(Row(message));
        qDebug() << "###Added message:" << message._text << "Level:" << getSeverityLevelName(message._level) << "Emitter:" << message._emitter;
    }
}

}
