// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ErrorLoggingConsentDialog.h"

#include <Application.h>

#include <QDesktopServices>

#ifdef _DEBUG
    #define ERROR_LOGGING_CONSENT_DIALOG_VERBOSE
#endif

using namespace mv;

ErrorLogging* ErrorLoggingConsentDialog::errorLoggingInstance = nullptr;

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

    if (!errorLoggingInstance->getUserHasOptedAction().isChecked())
		_buttonsLayout.addWidget(&_decideLaterPushButton);

    _layout.addLayout(&_buttonsLayout);

    setLayout(&_layout);

	connect(&_acceptPushButton, &QPushButton::clicked, this, [this]() -> void {
        errorLoggingInstance->getUserHasOptedAction().setChecked(true);
        errorLoggingInstance->getEnabledAction().setChecked(true);

        accept();
	});
    
	connect(&_optOutPushButton, &QPushButton::clicked, this, [this]() -> void {
        errorLoggingInstance->getUserHasOptedAction().setChecked(true);
        errorLoggingInstance->getEnabledAction().setChecked(false);

        accept();
	});

    connect(&_decideLaterPushButton, &QPushButton::clicked, this, [this]() -> void {
        reject();
	});
}

void ErrorLoggingConsentDialog::setErrorLoggingInstance(ErrorLogging* errorLoggingInstance)
{
    ErrorLoggingConsentDialog::errorLoggingInstance = errorLoggingInstance;
}
