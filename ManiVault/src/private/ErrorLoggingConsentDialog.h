// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <AbstractErrorLogger.h>

#include <QDialog>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

/**
 * Error logging consent dialog class
 *
 * Establish whether the user wants to opt in or out of (anonymous) error logging.
 *
 * @author Thomas Kroes
 */
class ErrorLoggingConsentDialog : public QDialog
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent widget
     * @param parent Pointer to parent widget (maybe nullptr)
     */
    explicit ErrorLoggingConsentDialog(QWidget* parent = nullptr);

private:
    QVBoxLayout     _layout;                    /** Main layout */
    QLabel          _notificationLabel;         /** Notification label */
    QHBoxLayout     _buttonsLayout;             /** Bottom buttons layout */
    QPushButton     _acceptPushButton;          /** Opt in of automated error reporting when clicked */
    QPushButton     _optOutPushButton;          /** Opt out of automated error reporting when clicked */
    QPushButton     _decideLaterPushButton;     /** Exit the dialog and show it at next startup */
};
