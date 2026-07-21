#pragma once

#include "ManiVaultGlobals.h"

#include <QString>

#include <exception>

class QWidget;

namespace mv {

class ManiVaultException;

namespace util {

/**
 * @brief Shows a modal exception dialog with a custom reason.
 *
 * The dialog displays the supplied reason, includes technical stack-trace
 * details when available from the active error manager, and offers a copy
 * action for the full report.
 *
 * @param title Dialog title.
 * @param reason Human-readable explanation of the error.
 * @param parent Optional parent widget.
 */
CORE_EXPORT void exceptionMessageBox(const QString& title, const QString& reason, QWidget* parent = nullptr);

/**
 * @brief Shows a modal exception dialog with a generic error message.
 * @param title Dialog title.
 * @param parent Optional parent widget.
 */
CORE_EXPORT void exceptionMessageBox(const QString& title, QWidget* parent = nullptr);

/**
 * @brief Shows a modal exception dialog for a standard C++ exception.
 * @param title Dialog title.
 * @param exception Exception whose what() text is shown as the error reason.
 * @param parent Optional parent widget.
 */
CORE_EXPORT void exceptionMessageBox(const QString& title, const std::exception& exception, QWidget* parent = nullptr);

/**
 * @brief Shows a modal exception dialog for a ManiVault exception.
 *
 * The dialog uses the exception message and stack trace carried by the
 * ManiVaultException instance.
 *
 * @param title Dialog title.
 * @param exception ManiVault exception to display.
 * @param parent Optional parent widget.
 */
CORE_EXPORT void exceptionMessageBox(const QString& title, const ManiVaultException& exception, QWidget* parent = nullptr);

} // namespace util

} // namespace mv
