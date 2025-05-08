// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AppFeatureAction.h"

using namespace mv::util;

namespace mv::gui {

AppFeatureAction::AppFeatureAction(QObject* parent, const QString& title) :
    HorizontalGroupAction(parent, title),
    _enabledAction(this, title, false),
    _summaryAction(this, "Summary"),
    _userHasOptedAction(this, "User has opted", false),
    _descriptionAction(this, "Description"),
    _settingsAction(this, QString("%1 Settings").arg(title))
{
    setShowLabels(false);

    auto settingsPrefix = QString("AppFeatures/%1/").arg(title);

    settingsPrefix.replace(" ", "");

    setSettingsPrefix(settingsPrefix);

    _enabledAction.setSettingsPrefix(QString("%1/Enabled").arg(WidgetAction::getSettingsPrefix()));
    _userHasOptedAction.setSettingsPrefix(QString("%1/UserHasOpted").arg(WidgetAction::getSettingsPrefix()));

    _enabledAction.setDefaultWidgetFlags(ToggleAction::WidgetFlag::ToggleImage);

    _summaryAction.setEnabled(false);
    _summaryAction.setDefaultWidgetFlags(StringAction::WidgetFlag::LineEdit);
    _summaryAction.setToolTip(QString("%1 summary").arg(title));
    _summaryAction.setStretch(1);

    _descriptionAction.setIconByName("circle-info");
    _descriptionAction.setToolTip(QString("%1 settings").arg(title));
    _descriptionAction.setConfigurationFlag(ConfigurationFlag::ForceCollapsedInGroup);
    _descriptionAction.setDefaultWidgetFlags(StringAction::WidgetFlag::TextBrowser);
    _descriptionAction.setPopupSizeHint(QSize(550, 350));

    _settingsAction.setVisible(false);
    _settingsAction.setIconByName("gear");
    _settingsAction.setToolTip(QString("%1 settings").arg(title));
    _settingsAction.setConfigurationFlag(ConfigurationFlag::ForceCollapsedInGroup);
    _settingsAction.setPopupSizeHint(QSize(400, 10));

    HorizontalGroupAction::addAction(&_enabledAction);
    HorizontalGroupAction::addAction(&_summaryAction);
    HorizontalGroupAction::addAction(&_descriptionAction);
    HorizontalGroupAction::addAction(&_settingsAction);

    const auto updateActionsReadOnly = [this]() -> void {
        _settingsAction.setEnabled(_enabledAction.isChecked());
        _descriptionAction.setEnabled(_enabledAction.isChecked());
	};

    updateActionsReadOnly();

    connect(&_enabledAction, &ToggleAction::toggled, this, [this, updateActionsReadOnly](bool toggled) -> void {
        
        if (toggled) {
            ConsentDialog consentDialog(this);

            if (consentDialog.exec() == QDialog::Accepted) {
                _userHasOptedAction.setChecked(true);
                _enabledAction.setChecked(true);
            }
            else {
                _enabledAction.setChecked(false);
            }
            _settingsAction.setEnabled(_enabledAction.isChecked());
            _descriptionAction.setEnabled(_enabledAction.isChecked());
        }

        updateActionsReadOnly();
        //mv::help().addNotification("App Feature", QString("%1 app feature has been %2").arg(text(), toggled ? "enabled" : "disabled"), StyledIcon(toggled ? "toggle-on" : "toggle-off"));
	});

    if (hasSetting(PrefixType::UserHasOpted)) {
        if (getUserHasOptedFromSettings()) {
            _enabledAction.setChecked(getEnabledFromSettings());
        }
    }
}

void AppFeatureAction::addAction(WidgetAction* action, std::int32_t widgetFlags /*= -1*/, WidgetConfigurationFunction widgetConfigurationFunction /*= WidgetConfigurationFunction()*/, bool load)
{
    _settingsAction.addAction(action, widgetFlags, widgetConfigurationFunction, load);
    _settingsAction.setVisible(true);
}

QString AppFeatureAction::getResourceLocation() const
{
    return _resourceLocation;
}

QString AppFeatureAction::getSettingsPrefix(const PrefixType& prefixType) const
{
	switch (prefixType) {
	    case PrefixType::Enabled:
	        return QString("AppFeatures/%1/Enabled").arg(text());

	    case PrefixType::UserHasOpted:
	        return QString("AppFeatures/%1/UserHasOpted").arg(text());
	}

    return {};
}

bool AppFeatureAction::hasSetting(const PrefixType& prefixType) const
{
    return Application::current()->hasSetting(getSettingsPrefix(prefixType));
}

void AppFeatureAction::loadDescriptionFromResource(const QString& resourceLocation)
{
    _resourceLocation = resourceLocation;

    QFile appFeatureHtmlFile(resourceLocation);

    if (appFeatureHtmlFile.open(QIODevice::ReadOnly)) {
        _descriptionAction.setString(appFeatureHtmlFile.readAll());
        appFeatureHtmlFile.close();
    }
    else {
        qWarning() << QString("No description available for %1: %2").arg(text(), appFeatureHtmlFile.errorString());
    }
}

bool AppFeatureAction::getEnabledFromSettings() const
{
    if (hasSetting(PrefixType::Enabled))
        return Application::current()->getSetting(getSettingsPrefix(PrefixType::Enabled)).toBool();

    return false;
}

bool AppFeatureAction::getUserHasOptedFromSettings() const
{
    if (hasSetting(PrefixType::UserHasOpted))
        return Application::current()->getSetting(getSettingsPrefix(PrefixType::UserHasOpted)).toBool();

    return false;
}

AppFeatureAction::ConsentDialog::ConsentDialog(AppFeatureAction* appFeatureAction, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _acceptPushButton("Accept"),
    _cancelPushButton("Cancel"),
    _decideLaterPushButton("Decide Later")
{
    setWindowModality(Qt::ApplicationModal);
    setWindowFlag(Qt::Dialog);
    setWindowFlag(Qt::WindowTitleHint);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowTitle(QString("Turn on %1 App Feature").arg(appFeatureAction->text()));
    setWindowIcon(StyledIcon("toggle-on"));
    move(QApplication::activeWindow()->geometry().center() - rect().center());

    _notificationLabel.setWordWrap(true);
    _notificationLabel.setOpenExternalLinks(true);

    QFile errorLoggingConsentHtmlFile(appFeatureAction->getResourceLocation());

    if (errorLoggingConsentHtmlFile.open(QIODevice::ReadOnly)) {
        _notificationLabel.setText(errorLoggingConsentHtmlFile.readAll());
        errorLoggingConsentHtmlFile.close();
    }

    _layout.addWidget(&_notificationLabel);
    _layout.addStretch(1);

    _buttonsLayout.setContentsMargins(9, 30, 9, 9);

    _buttonsLayout.addStretch(1);
    _buttonsLayout.addWidget(&_acceptPushButton);
    _buttonsLayout.addWidget(&_cancelPushButton);

    const auto& constErrorManager = mv::errors();

    if (!constErrorManager.getLoggingUserHasOptedAction().isChecked())
        _buttonsLayout.addWidget(&_decideLaterPushButton);

    _layout.addLayout(&_buttonsLayout);

    setLayout(&_layout);

    connect(&_acceptPushButton, &QPushButton::clicked, this, [this, &constErrorManager]() -> void {
        accept();
	});

    connect(&_cancelPushButton, &QPushButton::clicked, this, [this, &constErrorManager]() -> void {
        reject();
	});

    connect(&_decideLaterPushButton, &QPushButton::clicked, this, [this]() -> void {
        reject();
    });
}

QSize AppFeatureAction::ConsentDialog::sizeHint() const
{
    return { 500, 300 };
}

}
