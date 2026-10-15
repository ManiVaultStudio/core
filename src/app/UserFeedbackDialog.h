// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

/**
 * @brief Collects a standalone problem report or feature request from the user.
 *
 * The dialog validates the description and optional email address, then sends
 * the submission through the application's configured error logger.
 *
 * @author Thomas Kroes
 */
class UserFeedbackDialog : public QDialog
{
    Q_OBJECT

public:

    /**
     * @brief Constructs the standalone user-feedback dialog.
     * @param parent Pointer to the parent widget (maybe nullptr).
     */
    explicit UserFeedbackDialog(QWidget* parent = nullptr);

private:

    /**
     * @brief Updates submission availability from the current form contents.
     */
    void updateSubmitButton();

    /**
     * @brief Submits the form through the active error logger and reports the result.
     */
    void submit();

private:
    QVBoxLayout     _layout;                /**< Main dialog layout. */
    QLabel          _introductionLabel;     /**< Introductory explanation. */
    QLabel          _typeLabel;             /**< Feedback-type field label. */
    QComboBox       _typeComboBox;          /**< Problem or feature-request selector. */
    QLabel          _messageLabel;          /**< Description field label. */
    QPlainTextEdit  _messageTextEdit;       /**< User-provided feedback description. */
    QLabel          _emailLabel;            /**< Optional email field label. */
    QLineEdit       _emailLineEdit;         /**< Optional validated follow-up email. */
    QLabel          _privacyLabel;          /**< Link to the Sentry privacy policy. */
    QHBoxLayout     _buttonsLayout;          /**< Bottom action-button layout. */
    QPushButton     _submitButton;           /**< Submits the feedback. */
    QPushButton     _cancelButton;           /**< Closes the dialog without submission. */
};
