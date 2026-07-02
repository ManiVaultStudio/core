// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowResult.h"

#include "util/SeverityLevel.h"

#include "models/WorkflowMessagesListModel.h"
#include "models/WorkflowMessagesFilterModel.h"

#include <QDialog>
#include <QHash>
#include <QUuid>

namespace mv::workflow
{

/**
 * @brief Displays workflow result messages in a filterable dialog.
 *
 * WorkflowResultDialog presents the messages stored on a WorkflowResult using
 * the workflow message list and filter models. The dialog includes a severity
 * filter toolbar and a sortable tree view for inspecting message level, emitter,
 * location, text, details, and timestamp data.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowResultDialog : public QDialog
{
    Q_OBJECT

public:

    /**
     * @brief Constructs a workflow result dialog.
     *
     * The provided severity levels determine the initially selected message
     * filters. The dialog expects a valid workflow result; invalid results are
     * asserted in debug builds and leave the dialog uninitialized.
     *
     * @param workflowResult Workflow result whose messages should be displayed.
     * @param levels Initial severity levels to show.
     * @param parent Optional parent widget.
     */
    WorkflowResultDialog(const SharedWorkflowResult& workflowResult, util::SeverityLevels levels = util::allSeverityLevels, QWidget* parent = nullptr);

    /**
     * @brief Returns the preferred dialog size.
     * @return Preferred size for the workflow result dialog.
     */
    [[nodiscard]] QSize sizeHint() const override;

private:
    WorkflowMessagesListModel       _messagesListModel;       /**< Source model containing messages from the workflow result */
    WorkflowMessagesFilterModel     _messagesFilterModel;     /**< Proxy model used for severity filtering and sorting */

};

}
