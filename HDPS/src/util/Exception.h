// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QMessageBox>
#include <QDebug>

#include <stdexcept>

namespace hdps::util {

/**
 * Create an exception message box using a title and reason
 * @param title Message box title
 * @param reason Reason for the exception
 * @param parent Pointer to parent widget
 */
static void exceptionMessageBox(const QString& title, const QString& reason, QWidget* parent = nullptr)
{
    QMessageBox::critical(parent, title, reason);

    qDebug() << title << reason;
}

/**
 * Create an exception message box for unhandled exceptions
 * @param title Message box title
 * @param parent Pointer to parent widget
 */
static void exceptionMessageBox(const QString& title, QWidget* parent = nullptr)
{
    exceptionMessageBox(title, "An unhandled error occurred.", parent);
}

/**
 * Create an exception message box using title and exception
 * @param title Message box title
 * @param exception Reference to exception
 * @param parent Pointer to parent widget
 */
static void exceptionMessageBox(const QString& title, const std::exception& exception, QWidget* parent = nullptr)
{
    exceptionMessageBox(title, exception.what(), parent);
}

}
