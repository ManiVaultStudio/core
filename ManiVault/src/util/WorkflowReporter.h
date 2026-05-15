// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowExecutionContext.h"

#include <QString>

namespace mv::util
{

/**
 * @brief Utility class for reporting messages and progress within a workflow execution context.
 *
 * This class provides static methods for logging informational messages, warnings, and errors, as well as setting progress values.
 * The methods in this class will check if there is a current workflow execution context available and will delegate the reporting
 * to that context if it exists. This allows for consistent reporting of messages and progress across different parts of the workflow
 * execution, without requiring direct access to the workflow execution context in each part of the code.
 *
 * @author Thomas Kroes (BioVault: Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowReporter
{
public:

    /**
     * @brief Checks if there is a current workflow execution context available. This can be used to determine if reporting messages or setting progress will have an effect, as these operations require an active workflow execution context to function properly. If this method returns false, it means that there is no current workflow execution context, and any attempts to report messages or set progress will be ignored.
     * @return True if there is a current workflow execution context available, false otherwise.
     */
    static bool hasContext();

    /**
     * @brief Adds an informational message to the current workflow execution context, if available. The message includes the main text content, an optional location for additional context, and optional details as a QVariantMap for structured information. If there is no current workflow execution context, this method will have no effect.
     * @param text The main text or content of the informational message to be added to the current workflow execution context.
     * @param location The specific location in the code or workflow where the informational message was generated (e.g., function name, line number). This can be used to provide additional context for the message and help with debugging and tracing. This parameter is optional and can be left empty if not applicable.
     * @param details Additional details or metadata associated with the informational message. This parameter is optional and can be left empty if not applicable.
     */
    static void info(QString text, QString location = {}, QVariantMap details = {});

    /**
     * @brief Adds a warning message to the current workflow execution context, if available. The message includes the main text content, an optional location for additional context, and optional details as a QVariantMap for structured information. If there is no current workflow execution context, this method will have no effect.
     * @param text The main text or content of the warning message to be added to the current workflow execution context.
     * @param location The specific location in the code or workflow where the warning message was generated (e.g., function name, line number). This can be used to provide additional context for the message and help with debugging and tracing. This parameter is optional and can be left empty if not applicable.
     * @param details Additional details or metadata associated with the warning message. This parameter is optional and can be left empty if not applicable.
     */
    static void warning(QString text, QString location = {}, QVariantMap details = {});

    /**
     * @brief Adds an error message to the current workflow execution context, if available. The message includes the main text content, an optional location for additional context, and optional details as a QVariantMap for structured information. If there is no current workflow execution context, this method will have no effect.
     * @param text The main text or content of the error message to be added to the current workflow execution context.
     * @param location The specific location in the code or workflow where the error message was generated (e.g., function name, line number). This can be used to provide additional context for the message and help with debugging and tracing. This parameter is optional and can be left empty if not applicable.
     * @param details Additional details or metadata associated with the error message. This parameter is optional and can be left empty if not applicable.
     */
    static void error(QString text, QString location = {}, QVariantMap details = {});

    /**
     * @brief Adds a message with the specified severity level to the current workflow execution context, if available. The message includes the main text content, an optional location for additional context, and optional details as a QVariantMap for structured information. If there is no current workflow execution context, this method will have no effect.
     * @param severity The severity level of the message (e.g., Info, Warning, Error, Fatal). This determines how the message will be categorized and displayed in the workflow report.
     * @param text The main text or content of the message to be added to the current workflow execution context.
     * @param location The specific location in the code or workflow where the message was generated (e.g., function name, line number). This can be used to provide additional context for the message and help with debugging and tracing. This parameter is optional and can be left empty if not applicable.
     * @param details Additional details or metadata associated with the message. This parameter is optional and can be left empty if not applicable.
     */
    static void message(SeverityLevel severity, QString text, QString location = {}, QVariantMap details = {});

    // TODO
    static void setProgress(double value);
};

} // namespace mv::util