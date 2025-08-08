// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QMessageBox>
#include <QDebug>
#include <QException>
#include <QIcon>

#include <stdexcept>

namespace mv::util {

/**
 * Exception class for ManiVault
 *
 * Base class for exceptions in ManiVault.
 *
 * @author Thomas Kroes
 */
class BaseException : public QException
{
public:

	/**
     * Construct with \p message and \p icon
     * @param message Exception message
     * @param icon Icon to display in message box (default is empty)
	 */
	BaseException(const QString& message, const QIcon& icon = QIcon()) :
		_message(message),
		_icon(icon)
	{
    }

    /** Raises an exception, required by QException for rethrowing */
    void raise() const override { throw* this; }

    /** Clones an exception, required by QException for rethrowing */
    BaseException* clone() const override { return new BaseException(*this); }

    /**
     * Get the exception message
     * @return Exception message
     */
    QString getMessage() const { return _message; }

    /**
     * Get the icon to display in message box
     * @return Exception icon
     */
    QIcon getIcon() const { return _icon; }

private:
    QString _message;   /** The exception message */
    QIcon   _icon;      /** Icon to display in message box */
};

/**
 * Create an exception message box using a title and reason
 * @param title Message box title
 * @param reason Reason for the exception
 * @param parent Pointer to parent widget
 */
CORE_EXPORT inline void exceptionMessageBox(const QString& title, const QString& reason, QWidget* parent = nullptr)
{
    QMessageBox::critical(parent, title, reason);

    qDebug() << title << reason;
}

/**
 * Create an exception message box for unhandled exceptions
 * @param title Message box title
 * @param parent Pointer to parent widget
 */
CORE_EXPORT inline void exceptionMessageBox(const QString& title, QWidget* parent = nullptr)
{
    exceptionMessageBox(title, "An unhandled error occurred.", parent);
}

/**
 * Create an exception message box using title and exception
 * @param title Message box title
 * @param exception Reference to exception
 * @param parent Pointer to parent widget
 */
CORE_EXPORT inline void exceptionMessageBox(const QString& title, const std::exception& exception, QWidget* parent = nullptr)
{
    exceptionMessageBox(title, exception.what(), parent);
}

}
