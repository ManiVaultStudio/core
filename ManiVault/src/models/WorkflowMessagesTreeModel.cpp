// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowMessagesTreeModel.h"

#include <QDebug>

#ifdef _DEBUG
    #define WORKFLOW_MESSAGES_TREE_MODEL_VERBOSE
#endif

using namespace mv::workflow;

namespace mv {

WorkflowMessagesTreeModel::WorkflowMessagesTreeModel(QObject* parent /*= nullptr*/) :
    AbstractWorkflowMessagesModel(parent)
{
}

void WorkflowMessagesTreeModel::setWorkflowResult(const workflow::SharedWorkflowResult& workflowResult)
{
    beginResetModel();
    {
        auto messages = workflowResult->getMessages();

    	for (const auto& message : messages) {

            const auto matches = match(index(0, static_cast<int>(Column::ID)), Qt::DisplayRole, message.parentId, 1, Qt::MatchExactly | Qt::MatchRecursive);

            if (!matches.isEmpty()) {
                const auto parentIndex  = matches.first().siblingAtColumn(0);
                
                if (const auto parentItem = itemFromIndex(parentIndex)) {
                    parentItem->appendRow(Row(message));
                }
            } else {
                appendRow(Row(message));
            }
        }
    }
    endResetModel();
}

}
