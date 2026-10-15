// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "util/SeverityLevel.h"
#include "util/StackFrame.h"

#include <QObject>
#include <QVariantMap>
#include <QUuid>

#include <source_location>

namespace mv {

/**
 * @brief Exception type carrying ManiVault diagnostic context.
 *
 * ManiVaultException extends std::runtime_error with severity, user-facing
 * message text, technical cause, source/context location, structured details,
 * a diagnostic identifier, and captured debug stack trace metadata. It is used
 * when errors need to be shown to users and reported with enough context for
 * diagnostics.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT ManiVaultException : public std::runtime_error {
public:

    /**
     * @brief Constructs a ManiVault exception.
     *
     * The constructor stores source-location metadata, creates a diagnostic id,
     * and appends the current debug stack trace to the details map.
     *
     * @param severity Severity associated with the exception.
     * @param message User-facing exception message.
     * @param what Technical cause or short diagnostic description.
     * @param where Optional source or workflow context where the error occurred.
     * @param details Optional structured diagnostic details.
     * @param location C++ source location captured at the throw site.
     */
    ManiVaultException(util::SeverityLevel severity, QString message, QString what, QString where = {}, QVariantMap details = {}, std::source_location location = std::source_location::current());

    /**
     * @brief Returns a copy of this exception with additional details.
     *
     * Additional detail keys overwrite existing keys with the same name.
     *
     * @param additionalDetails Details to merge into the copied exception.
     * @return New exception containing the merged details.
     */
    ManiVaultException withAddedDetails(const QVariantMap& additionalDetails) const;

public: // Getters

    /**
     * @brief Returns the exception severity.
     * @return Severity level.
     */
    [[nodiscard]] util::SeverityLevel getSeverity() const;

    /**
     * @brief Returns the user-facing exception message.
     * @return Exception message.
     */
    [[nodiscard]] QString getMessage() const;

    /**
     * @brief Returns the technical exception cause.
     * @return Technical exception text.
     */
    [[nodiscard]] QString getWhat() const;

    /**
     * @brief Returns the source or context where the exception occurred.
     * @return Context location string.
     */
    [[nodiscard]] QString getWhere() const;

    /**
     * @brief Returns structured diagnostic details.
     * @return Diagnostic details map.
     */
    [[nodiscard]] QVariantMap getDetails() const;

    /**
     * @brief Returns the unique diagnostic identifier.
     * @return Diagnostic id.
     */
    [[nodiscard]] QUuid getDiagnosticId() const;

    /**
     * @brief Returns the captured debug stack trace.
     * @return Stack trace frames.
     */
    [[nodiscard]] util::StackTrace getStackTrace() const;

private:
    util::SeverityLevel     _severity;      /**< Severity level of the exception */
    QString                 _message;       /**< User-facing exception message */
    QString                 _what;          /**< Technical cause or diagnostic description */
    QString                 _where;         /**< Source or context where the exception occurred */
    QVariantMap             _details;       /**< Structured diagnostic details */
    QUuid                   _diagnosticId;  /**< Unique diagnostic identifier for the exception */
};

}
