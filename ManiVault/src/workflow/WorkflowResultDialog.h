// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowResult.h"
#include "WorkflowOptions.h"

#include "util/SeverityLevel.h"

#include "models/WorkflowMessagesListModel.h"
#include "models/WorkflowMessagesFilterModel.h"

#include <QDialog>
#include <QHash>
#include <QUuid>
#include <QMutex>
#include <QMutexLocker>
#include <QTreeView>


namespace mv::workflow
{

/**
 * @brief Dialog that displays workflow result messages.
 */
class CORE_EXPORT WorkflowResultDialog : public QDialog
{
    Q_OBJECT

public:

    /**
     * @brief Constructs a workflow result dialog.
     * @param workflowResult Result whose messages should be displayed.
     * @param levels Message severity levels to show.
     * @param parent Optional parent widget.
     */
    WorkflowResultDialog(const SharedWorkflowResult& workflowResult, util::SeverityLevels levels = util::allSeverityLevels, QWidget* parent = nullptr);

    /** @return Preferred dialog size. */
    QSize sizeHint() const override;

private:

    WorkflowMessagesListModel       _messagesListModel;    /**< Source model containing workflow messages */
    WorkflowMessagesFilterModel     _messagesFilterModel;  /**< Filter model for visible severity levels */

};

}
