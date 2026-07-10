// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>

namespace mv::workflow
{

/**
 * @brief Options for customizing the details of a workflow result.
 */
struct CORE_EXPORT WorkflowResultDetailsOptions
{
    QString operationName;      /**< Operation label, such as "Save project" or "Open project" */
    QString subjectName;        /**< Subject label, such as a project filename */

    QString overrideTitle;      /**< Full result-dialog title override */
    QString overrideMessage;    /**< Full result-dialog intro message override */

    /**
     * @brief Creates the result-dialog title.
     * @param workflowName Fallback workflow name.
     * @return Dialog title.
     */
    QString makeTitle(const QString& workflowName) const
    {
        if (!overrideTitle.isEmpty())
            return overrideTitle;

        if (!operationName.isEmpty())
            return operationName;

        return QStringLiteral("Workflow result - %1").arg(workflowName);
    }

    /**
     * @brief Creates the result-dialog intro message.
     * @param workflowName Fallback workflow name.
     * @return Dialog message.
     */
    QString makeMessage(const QString& workflowName) const
    {
        if (!overrideMessage.isEmpty())
            return overrideMessage;

        if (!operationName.isEmpty() && !subjectName.isEmpty())
            return QStringLiteral("Diagnostic messages while running '%1' for '%2':").arg(operationName, subjectName);

        if (!operationName.isEmpty())
            return QStringLiteral("Diagnostic messages while running '%1':").arg(operationName);

        return QStringLiteral("Diagnostic messages by workflow '%1':").arg(workflowName);
    }

    /**
     * @brief Creates the subject line for the result-dialog.
     * @return Subject line, or empty if no subject is set.
     */
    QString makeSubjectLine() const
    {
        if (subjectName.isEmpty())
            return {};

        return QStringLiteral("Subject: %1").arg(subjectName);
    }
};

}
