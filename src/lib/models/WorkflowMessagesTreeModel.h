// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractScriptsModel.h"
#include "AbstractWorkflowMessagesModel.h"

namespace mv {

/**
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT WorkflowMessagesTreeModel : public AbstractWorkflowMessagesModel
{
public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    WorkflowMessagesTreeModel(QObject* parent = nullptr);

    void setWorkflowResult(const workflow::SharedWorkflowResult& workflowResult);

};

}
