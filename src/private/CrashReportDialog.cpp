// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "CrashReportDialog.h"

#include <util/StyledIcon.h>

#include <QApplication>
#include <QClipboard>
#include <QPainter>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QTimer>

#ifdef _DEBUG
    #define CRASH_REPORT_DIALOG_VERBOSE
#endif

using namespace mv::util;

namespace
{
    QIcon createStandaloneFontAwesomeIcon(const QString& iconName)
    {
        QPixmap pixmap(64, 64);

        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);

        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QApplication::palette().color(QPalette::WindowText));
        painter.setFont(StyledIcon::getIconFont(42));
        painter.drawText(pixmap.rect(), Qt::AlignCenter, StyledIcon::getIconCharacter(iconName));

        return QIcon(pixmap);
    }
}

QIcon CrashReportDialog::windowIcon         = QIcon();
QIcon CrashReportDialog::exclamationIcon    = QIcon();
QIcon CrashReportDialog::copyIcon           = QIcon();
QIcon CrashReportDialog::checkIcon          = QIcon();

CrashReportDialog::CrashReportDialog(const QString& eventId, QWidget* parent):
    QDialog(parent),
    _notificationLabel("ManiVault Studio closed unexpectedly. Because error reporting is enabled, the technical crash report has already been sent automatically. You can optionally help us improve by providing information about what happened.\n"),
    _feedbackLabel("What were you doing before ManiVault Studio closed?"),
    _contactLabel("\nYour email (optional):"),
    _privacyLabel("Crash data is processed according to the <a href=\"https://sentry.io/privacy/\">Sentry privacy policy</a>."),
    _eventIdTitleLabel("Crash event ID:"),
    _eventIdLabel(eventId),
    _sendButton("Submit feedback"),
    _restartButton("Restart ManiVault Studio"),
    _cancelButton("Close"),
    _submitFeedback(false),
    _restartApplication(false),
    _eventId(eventId)
{
    setWindowTitle("Crash report");
    setWindowModality(Qt::ApplicationModal);
    setWindowFlag(Qt::Dialog);
    setWindowFlag(Qt::WindowTitleHint);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowIcon(windowIcon);

    _notificationLabel.setWordWrap(true);

    _layout.addWidget(&_notificationLabel);

    _layout.addWidget(&_feedbackLabel);

    _feedbackTextEdit.setPlaceholderText("Describe the steps leading up to the crash...");

    _layout.addWidget(&_feedbackTextEdit);

    _layout.addWidget(&_contactLabel);

    QRegularExpression emailRegularExpression("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,63}\\b", QRegularExpression::CaseInsensitiveOption);

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

        updateSubmitButton();
    });

    connect(&_feedbackTextEdit, &QPlainTextEdit::textChanged, this, &CrashReportDialog::updateSubmitButton);

    _layout.addWidget(&_contactLineEdit);

    _privacyLabel.setOpenExternalLinks(true);
    _privacyLabel.setTextInteractionFlags(Qt::TextBrowserInteraction);

    auto eventIdTitleFont = _eventIdTitleLabel.font();

    eventIdTitleFont.setWeight(QFont::DemiBold);
    _eventIdTitleLabel.setFont(eventIdTitleFont);

    _eventIdLabel.setTextFormat(Qt::PlainText);
    _eventIdLabel.setTextInteractionFlags(Qt::TextSelectableByMouse);

    _eventIdSeparator.setFrameShape(QFrame::VLine);
    _eventIdSeparator.setFrameShadow(QFrame::Sunken);

    _copyEventIdButton.setIcon(copyIcon);
    _copyEventIdButton.setAutoRaise(true);
    _copyEventIdButton.setToolTip("Copy crash event ID");

    _eventIdLayout.setContentsMargins(0, 0, 0, 0);
    _eventIdLayout.addWidget(&_eventIdTitleLabel);
    _eventIdLayout.addWidget(&_eventIdLabel);
    _eventIdLayout.addStretch(1);
    _eventIdLayout.addWidget(&_eventIdSeparator);
    _eventIdLayout.addWidget(&_copyEventIdButton);

    _layout.addWidget(&_privacyLabel);
    _layout.addLayout(&_eventIdLayout);

    _buttonsLayout.addStretch(1);
    _buttonsLayout.addWidget(&_sendButton);
    _buttonsLayout.addWidget(&_restartButton);
    _buttonsLayout.addWidget(&_cancelButton);

    _layout.addLayout(&_buttonsLayout);

    setLayout(&_layout);

    updateSubmitButton();

    connect(&_sendButton, &QPushButton::clicked, this, [this] {
        _submitFeedback = true;
        accept();
    });

    connect(&_restartButton, &QPushButton::clicked, this, [this] {
        _submitFeedback = _sendButton.isEnabled();
        _restartApplication = true;
        accept();
    });

    connect(&_copyEventIdButton, &QToolButton::clicked, this, [this] {
        QApplication::clipboard()->setText(_eventId);
        _copyEventIdButton.setIcon(checkIcon);
        _copyEventIdButton.setToolTip("Copied");

        QTimer::singleShot(2000, this, [this] {
            _copyEventIdButton.setIcon(copyIcon);
            _copyEventIdButton.setToolTip("Copy crash event ID");
        });
    });

    connect(&_cancelButton, &QPushButton::clicked, this, &CrashReportDialog::reject);
}

CrashReportDialog::CrashUserInfo CrashReportDialog::getCrashUserInfo() const
{
    return {
        _submitFeedback,
        _restartApplication,
        _feedbackTextEdit.toPlainText(),
        _contactLineEdit.hasAcceptableInput() ? _contactLineEdit.text() : QString()
    };
}

void CrashReportDialog::updateSubmitButton()
{
    const auto hasFeedback = !_feedbackTextEdit.toPlainText().trimmed().isEmpty();
    const auto hasValidEmail = !_contactLineEdit.text().trimmed().isEmpty() && _contactLineEdit.hasAcceptableInput();

    _sendButton.setEnabled(hasFeedback || hasValidEmail);
}

void CrashReportDialog::initialize()
{
    windowIcon      = createStandaloneFontAwesomeIcon("bug");
    exclamationIcon = createStandaloneFontAwesomeIcon("exclamation");
    copyIcon        = createStandaloneFontAwesomeIcon("copy");
    checkIcon       = createStandaloneFontAwesomeIcon("check");
}
