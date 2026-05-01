// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowMessagesFilterModel.h"

#ifdef _DEBUG
    #define WORKFLOW_MESSAGES_FILTER_MODEL_VERBOSE
#endif

namespace mv {

WorkflowMessagesFilterModel::WorkflowMessagesFilterModel(QObject* parent /*= nullptr*/) :
    SortFilterProxyModel(parent)
{
}

}
