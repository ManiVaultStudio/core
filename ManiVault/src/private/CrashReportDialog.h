// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QDialog>
#include <QFrame>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QToolButton>
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
        bool        _submitFeedback;    /** Whether the user chose to submit optional feedback */
        bool        _restartApplication;/** Whether the user chose to restart ManiVault Studio */
        QString     _feedback;          /** Feedback on the crash */
        QString     _contactDetails;    /** Contact details of the issuer */
    };

public:

    /**
     * Construct with pointer to \p parent widget
     * @param parent Pointer to parent widget (maybe nullptr)
     */
    explicit CrashReportDialog(const QString& eventId, QWidget* parent = nullptr);

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
    /** Updates feedback submission availability from the current input. */
    void updateSubmitButton();

private:
    QVBoxLayout     _layout;                /** Main layout */
    QLabel          _notificationLabel;     /** Notification label */
    QLabel          _feedbackLabel;         /** Feedback label */
    QLabel          _contactLabel;          /** Contact label */
    QLabel          _privacyLabel;          /** Link to the Sentry privacy policy */
    QLabel          _eventIdTitleLabel;     /** Crash event identifier title */
    QLabel          _eventIdLabel;          /** Crash event identifier */
    QPlainTextEdit  _feedbackTextEdit;      /** Feedback text multi line input */
    QLineEdit       _contactLineEdit;       /** Issuer contact details single line text input */
    QHBoxLayout     _eventIdLayout;          /** Crash event identifier and copy action layout */
    QFrame          _eventIdSeparator;       /** Separator before the copy action */
    QToolButton     _copyEventIdButton;      /** Copies the crash event identifier */
    QHBoxLayout     _buttonsLayout;         /** Bottom buttons layout */
    QPushButton     _sendButton;            /** Sends the crash report when triggered  */
    QPushButton     _restartButton;         /** Restarts ManiVault Studio */
    QPushButton     _cancelButton;          /** Cancels the dialog when triggered */
    QAction         _trailingAction;        /** Action at the end of the contact line input */
    bool            _submitFeedback;        /** Whether optional feedback should be submitted */
    bool            _restartApplication;    /** Whether ManiVault Studio should be restarted */
    QString         _eventId;                /** Crash event identifier */

    static QIcon    windowIcon;         /** Window bug icon */
    static QIcon    exclamationIcon;    /** Exclamation icon */
    static QIcon    copyIcon;           /** Copy-to-clipboard icon */
    static QIcon    checkIcon;          /** Successful-copy icon */
};
