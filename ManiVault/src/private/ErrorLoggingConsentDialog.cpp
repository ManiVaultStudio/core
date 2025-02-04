// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ErrorLoggingConsentDialog.h"

#include "ErrorLogging.h"

#include <Application.h>

#include <QDesktopServices>

#ifdef _DEBUG
    #define ERROR_LOGGING_CONSENT_DIALOG_VERBOSE
#endif

using namespace mv;

ErrorLoggingConsentDialog::ErrorLoggingConsentDialog(QWidget* parent):
	QDialog(parent),
    _acceptPushButton("Accept"),
    _optOutPushButton( "Opt Out"),
    _decideLaterPushButton("Decide Later")
{
	setWindowTitle("Help us improve ManiVault Studio");
    setWindowModality(Qt::ApplicationModal);
    setWindowFlag(Qt::Dialog);
    setWindowFlag(Qt::WindowTitleHint);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowFlag(Qt::WindowCloseButtonHint, false);
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("check-square"));
    setFixedSize({ 500, 420 });

    _notificationLabel.setWordWrap(true);
    _notificationLabel.setOpenExternalLinks(true);

    QFile errorLoggingConsentHtmlFile(":/HTML/ErrorLoggingConsent");

    if (errorLoggingConsentHtmlFile.open(QIODevice::ReadOnly)) {
        _notificationLabel.setText(errorLoggingConsentHtmlFile.readAll());
        errorLoggingConsentHtmlFile.close();
	}

    _layout.addWidget(&_notificationLabel);
    _layout.addStretch(1);

    _buttonsLayout.addStretch(1);
    _buttonsLayout.addWidget(&_acceptPushButton);
    _buttonsLayout.addWidget(&_optOutPushButton);

    if (!ErrorLogging::getUserHasOpted())
		_buttonsLayout.addWidget(&_decideLaterPushButton);

    _layout.addLayout(&_buttonsLayout);

    setLayout(&_layout);

	connect(&_acceptPushButton, &QPushButton::clicked, this, [this]() -> void {
        ErrorLogging::setUserHasOpted(true);
        ErrorLogging::setErrorLoggingEnabled(true);

        accept();
	});
    
	connect(&_optOutPushButton, &QPushButton::clicked, this, [this]() -> void {
        ErrorLogging::setUserHasOpted(true);
        ErrorLogging::setErrorLoggingEnabled(false);

        accept();
	});

    connect(&_decideLaterPushButton, &QPushButton::clicked, this, [this]() -> void {
        reject();
	});
}
