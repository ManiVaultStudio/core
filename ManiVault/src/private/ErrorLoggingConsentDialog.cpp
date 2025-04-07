// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ErrorLoggingConsentDialog.h"
#include "AbstractErrorLogger.h"

#include <Application.h>

#include <QDesktopServices>

#ifdef _DEBUG
    #define ERROR_LOGGING_CONSENT_DIALOG_VERBOSE
#endif

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

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
    setWindowIcon(StyledIcon("check-square"));
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

    const auto& constErrorManager = mv::errors();

    if (!constErrorManager.getLoggingUserHasOptedAction().isChecked())
		_buttonsLayout.addWidget(&_decideLaterPushButton);

    _layout.addLayout(&_buttonsLayout);

    setLayout(&_layout);

    _acceptPushButton.setEnabled(!constErrorManager.getLoggingUserHasOptedAction().isChecked() || !constErrorManager.getLoggingEnabledAction().isChecked());
    _optOutPushButton.setEnabled(!constErrorManager.getLoggingUserHasOptedAction().isChecked() || constErrorManager.getLoggingEnabledAction().isChecked());

	connect(&_acceptPushButton, &QPushButton::clicked, this, [this, &constErrorManager]() -> void {
        const_cast<mv::gui::ToggleAction&>(constErrorManager.getLoggingUserHasOptedAction()).setChecked(true);
        const_cast<mv::gui::ToggleAction&>(constErrorManager.getLoggingEnabledAction()).setChecked(true);

        accept();
	});
    
	connect(&_optOutPushButton, &QPushButton::clicked, this, [this, &constErrorManager]() -> void {
        const_cast<mv::gui::ToggleAction&>(constErrorManager.getLoggingUserHasOptedAction()).setChecked(true);
        const_cast<mv::gui::ToggleAction&>(constErrorManager.getLoggingEnabledAction()).setChecked(false);

        accept();
	});

    connect(&_decideLaterPushButton, &QPushButton::clicked, this, [this]() -> void {
        reject();
	});
}
