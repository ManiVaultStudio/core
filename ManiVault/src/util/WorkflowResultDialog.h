// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowResult.h"

#include "models/WorkflowMessagesListModel.h"
#include "models/WorkflowMessagesFilterModel.h"

#include "widgets/HierarchyWidget.h"

#include <QDialog>
#include <QHash>
#include <QUuid>

namespace mv::util
{

class CORE_EXPORT WorkflowResultDialog : public QDialog
{
    Q_OBJECT

public:

    WorkflowResultDialog(const SharedWorkflowResult& workflowResult, WorkflowMessageLevels levels = allWorkflowMessageTypes, QWidget* parent = nullptr);

    QSize sizeHint() const override;

private:
    WorkflowMessagesListModel       _messagesListModel;
    WorkflowMessagesFilterModel     _messagesFilterModel;
    gui::HierarchyWidget            _hierarchyWidget;

};

} // namespace mv::util