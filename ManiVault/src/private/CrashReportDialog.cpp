// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "CrashReportDialog.h"

#include <Application.h>

#ifdef _DEBUG
    #define CRASH_REPORT_DIALOG_VERBOSE
#endif

using namespace mv;
using namespace mv::util;

QIcon CrashReportDialog::windowIcon         = QIcon();
QIcon CrashReportDialog::frownIcon          = QIcon();
QIcon CrashReportDialog::exclamationIcon    = QIcon();

CrashReportDialog::CrashReportDialog(QWidget* parent):
	QDialog(parent),
    _notificationLabel("ManiVault Studio has encountered an error and needs to close. Please help us improve by providing some information about what happened.\n"),
    _feedbackLabel("What were you doing when the crash occurred?"),
    _contactLabel("\nYour email (optional):"),
    _sendButton("Send report"),
    _cancelButton("Cancel")
{
	setWindowTitle("Crash report");
    setWindowModality(Qt::ApplicationModal);
    setWindowFlag(Qt::Dialog);
    setWindowFlag(Qt::WindowTitleHint);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowIcon(windowIcon);

    _notificationIcon.setPixmap(frownIcon.pixmap(QSize(32, 32)));
    _notificationIcon.setAlignment(Qt::AlignTop);

    _notificationLabel.setWordWrap(true);

    _notificationLayout.addWidget(&_notificationIcon);
    _notificationLayout.addWidget(&_notificationLabel, 1);

    _layout.addLayout(&_notificationLayout);

    _layout.addWidget(&_feedbackLabel);

	_feedbackTextEdit.setPlaceholderText("Describe the steps leading to the crash...");

    _layout.addWidget(&_feedbackTextEdit);

    _layout.addWidget(&_contactLabel);

    QRegularExpression emailRegularExpression("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b", QRegularExpression::CaseInsensitiveOption);

    _contactLineEdit.setValidator(new QRegularExpressionValidator(emailRegularExpression, this));
	_contactLineEdit.setPlaceholderText("Enter your email if you'd like us to follow up...");
	_contactLineEdit.addAction(&_trailingAction, QLineEdit::TrailingPosition);

    _trailingAction.setIcon(exclamationIcon);
    _trailingAction.setVisible(false);

	connect(&_contactLineEdit, &QLineEdit::textChanged, this, [this]() -> void {
        if (!_contactLineEdit.text().isEmpty() && !_contactLineEdit.hasAcceptableInput()) {
            _trailingAction.setVisible(true);
        } else {
            _trailingAction.setVisible(false);
        }
	});

    _layout.addWidget(&_contactLineEdit);

    // TODO
    // _buttonsLayout.addWidget(const_cast<gui::ToggleAction&>(mv::settings().getErrorLoggingSettingsAction().getShowCrashReportDialogAction()).createWidget(this));
    _buttonsLayout.addStretch(1);
    _buttonsLayout.addWidget(&_sendButton);
    _buttonsLayout.addWidget(&_cancelButton);

    _layout.addLayout(&_buttonsLayout);

    setLayout(&_layout);

	connect(&_sendButton, &QPushButton::clicked, this, &CrashReportDialog::accept);
	connect(&_cancelButton, &QPushButton::clicked, this, &CrashReportDialog::reject);
}

CrashReportDialog::CrashUserInfo CrashReportDialog::getCrashUserInfo() const
{
    return {
        result() == DialogCode::Accepted,
        _feedbackTextEdit.toPlainText(),
        _contactLineEdit.text()
    };
}

void CrashReportDialog::initialize()
{
    windowIcon      = StyledIcon("bug");
    frownIcon       = StyledIcon("frown");
    exclamationIcon = StyledIcon("exclamation");
}
