// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QAction>

/**
 * Crash report dialog class
 *
 * Gather additional information from the user regarding the crash.
 *
 * @author Thomas Kroes
 */
class CrashReportDialog : public QDialog
{
    Q_OBJECT

public:

    /** Crash user-supplied additional information */
    struct CrashUserInfo
    {
        bool        _sendReport;        /** Boolean determining whether to send a crash report or not */
        QString     _feedback;          /** Feedback on the crash */
        QString     _contactDetails;    /** Contact details of the issuer */
    };

public:

    /**
     * Construct with pointer to \p parent widget
     * @param parent Pointer to parent widget (maybe nullptr)
     */
    explicit CrashReportDialog(QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return { 640, 480 };
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

    /**
     * Get crash info provided by the user
     * @return Crash info
     */
    CrashUserInfo getCrashUserInfo() const;

    /** For initializing icons etc. */
    static void initialize();

private:
    QVBoxLayout     _layout;                /** Main layout */
    QHBoxLayout     _notificationLayout;    /** Notification layout */
    QLabel          _notificationIcon;      /** Notification icon */
    QLabel          _notificationLabel;     /** Notification label */
    QLabel          _feedbackLabel;         /** Feedback label */
    QLabel          _contactLabel;          /** Contact label */
    QPlainTextEdit  _feedbackTextEdit;      /** Feedback text multi line input */
    QLineEdit       _contactLineEdit;       /** Issuer contact details single line text input */
    QHBoxLayout     _buttonsLayout;         /** Bottom buttons layout */
    QPushButton     _sendButton;            /** Sends the crash report when triggered  */
    QPushButton     _cancelButton;          /** Cancels the dialog when triggered */
    QAction         _trailingAction;        /** Action at the end of the contact line input */

    static QIcon    windowIcon;         /** Window bug icon */
    static QIcon    frownIcon;          /** Frown icon */
    static QIcon    exclamationIcon;    /** Exclamation icon */
};
