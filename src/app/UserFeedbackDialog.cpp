// SPDX-License-Identifier: LGPL-3.0-or-later

#include "UserFeedbackDialog.h"

#include "CoreInterface.h"

#include <util/StyledIcon.h>

#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

using namespace mv;
using namespace mv::util;

UserFeedbackDialog::UserFeedbackDialog(QWidget* parent) :
    QDialog(parent),
    _introductionLabel("Tell us about a problem you encountered or an improvement you would like to see in ManiVault Studio."),
    _typeLabel("Feedback type:"),
    _messageLabel("Description:"),
    _emailLabel("Your email (optional):"),
    _privacyLabel("Your feedback is sent to Sentry and processed according to the <a href=\"https://sentry.io/privacy/\">Sentry privacy policy</a>."),
    _submitButton("Submit feedback"),
    _cancelButton("Cancel")
{
    setWindowTitle("Send feedback");
    setWindowIcon(StyledIcon("comment-dots"));
    setMinimumSize(600, 440);

    _introductionLabel.setWordWrap(true);
    _typeComboBox.addItem(StyledIcon("bug"), "Report a problem", "Problem report");
    _typeComboBox.addItem(StyledIcon("lightbulb"), "Request a feature", "Feature request");
    _messageTextEdit.setPlaceholderText("Describe what happened, or explain the feature and how it would help...");
    _emailLineEdit.setPlaceholderText("Enter your email if you would like us to follow up...");
    _emailLineEdit.setValidator(new QRegularExpressionValidator(QRegularExpression("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,63}\\b", QRegularExpression::CaseInsensitiveOption), this));
    _privacyLabel.setOpenExternalLinks(true);
    _privacyLabel.setTextInteractionFlags(Qt::TextBrowserInteraction);

    _layout.addWidget(&_introductionLabel);
    _layout.addSpacing(8);
    _layout.addWidget(&_typeLabel);
    _layout.addWidget(&_typeComboBox);
    _layout.addWidget(&_messageLabel);
    _layout.addWidget(&_messageTextEdit, 1);
    _layout.addWidget(&_emailLabel);
    _layout.addWidget(&_emailLineEdit);
    _layout.addWidget(&_privacyLabel);

    _buttonsLayout.addStretch(1);
    _buttonsLayout.addWidget(&_submitButton);
    _buttonsLayout.addWidget(&_cancelButton);
    _layout.addLayout(&_buttonsLayout);
    setLayout(&_layout);

    connect(&_messageTextEdit, &QPlainTextEdit::textChanged, this, &UserFeedbackDialog::updateSubmitButton);
    connect(&_emailLineEdit, &QLineEdit::textChanged, this, &UserFeedbackDialog::updateSubmitButton);
    connect(&_submitButton, &QPushButton::clicked, this, &UserFeedbackDialog::submit);
    connect(&_cancelButton, &QPushButton::clicked, this, &UserFeedbackDialog::reject);
    updateSubmitButton();
}

void UserFeedbackDialog::updateSubmitButton()
{
    const auto emailIsValid = _emailLineEdit.text().trimmed().isEmpty() || _emailLineEdit.hasAcceptableInput();
    _submitButton.setEnabled(!_messageTextEdit.toPlainText().trimmed().isEmpty() && emailIsValid);
}

void UserFeedbackDialog::submit()
{
    const auto type = _typeComboBox.currentData().toString();

    if (!errors().submitUserFeedback(type, _messageTextEdit.toPlainText(), _emailLineEdit.text())) {
        QMessageBox::warning(this, "Feedback could not be sent", "Error reporting is disabled or unavailable. Enable error reporting in the application settings and try again.");
        return;
    }

    QMessageBox::information(this, "Feedback submitted", "Thank you. Your feedback has been submitted successfully.");
    accept();
}
