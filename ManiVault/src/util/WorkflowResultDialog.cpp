// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowResultDialog.h"

namespace mv::util
{

WorkflowResultDialog::WorkflowResultDialog(const SharedWorkflowResult& workflowResult, QWidget* parent) :
	QDialog(parent)
{
    Q_ASSERT(workflowResult);

    if (!workflowResult) {
        qDebug() << "WorkflowResultDialog: Invalid workflow result provided";
        return;
    }

    setWindowTitle("Workflow result - " + workflowResult->getWorkflowName());
    setWindowIcon(StyledIcon("scroll"));


}

}
