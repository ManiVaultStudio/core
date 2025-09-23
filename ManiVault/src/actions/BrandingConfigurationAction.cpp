// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "BrandingConfigurationAction.h"

using namespace mv::util;

namespace mv::gui {

BrandingConfigurationAction::BrandingConfigurationAction(QObject* parent, const QString& title) :
    VerticalGroupAction(parent, title),
    _organizationNameAction(this, "Organization name"),
    _organizationDomainAction(this, "Organization domain"),
    _baseNameAction(this, "Base name"),
    _fullNameAction(this, "Full name"),
    _editFullNameAction(this, "Edit full name"),
    _logoAction(this, "Logo"),
    _iconAction(this, "Icon"),
    _overrideIconAction(this, "Override icon"),
	_iconFromLogoAction(this, "Icon from logo"),
    _splashScreenAction(this),
    _aboutAction(this, "About")
{
    _organizationNameAction.setToolTip("The name of the organization (primarily used for storing settings)");
    _organizationDomainAction.setToolTip("The domain of the organization (primarily used for storing settings)");
    _baseNameAction.setToolTip("The name of the application without the version (show in the titlebar and used in storing settings)");
    _fullNameAction.setToolTip("The full name of the application (might include version))");
    _editFullNameAction.setToolTip("Override the auto-generated full name of the application");
    _logoAction.setToolTip("Set the logo");
    _iconAction.setToolTip("Change the application icon");
    _overrideIconAction.setToolTip("Override the default application icon");
    _iconFromLogoAction.setToolTip("Use the logo to generate the application icon");
    _splashScreenAction.setToolTip("Configure the splash screen");
    _aboutAction.setToolTip("Overrides the default (HTML) text in the about dialog");

    _logoAction.setDefaultWidgetFlags(ImageAction::WidgetFlag::Loader);

    _editFullNameAction.setChecked(false);

    _aboutAction.setDefaultWidgetFlags(StringAction::WidgetFlag::TextEdit);

    addAction(&_organizationNameAction);
    addAction(&_organizationDomainAction);
    addAction(&_baseNameAction);
    addAction(&_fullNameAction);
    addAction(&_editFullNameAction);
    addAction(&_logoAction);
    addAction(&_iconAction);
    addAction(&_iconFromLogoAction);
    addAction(&_splashScreenAction);
    addAction(&_aboutAction);

    const auto updateFullNameAction = [this]() -> void {
        _fullNameAction.setEnabled(_editFullNameAction.isChecked());

        if (!_editFullNameAction.isChecked())
            _fullNameAction.setString(QString("%1 %2").arg(_baseNameAction.getString(), QString::fromStdString(Application::current()->getVersion().getVersionString())));
    };

    updateFullNameAction();

    connect(&_editFullNameAction, &ToggleAction::toggled, this, updateFullNameAction);
    connect(&_baseNameAction, &StringAction::stringChanged, this, updateFullNameAction);

    const auto updateApplicationIconAction = [this]() -> void {
        _iconAction.setEnabled(_iconFromLogoAction.isChecked());
	};

    updateApplicationIconAction();

    connect(&_iconFromLogoAction, &ToggleAction::toggled, this, updateApplicationIconAction);

    _aboutAction.setString(QString("%3 is a flexible and extensible visual analytics framework for high-dimensional data.<br> <br>"
        "For more information, please visit: <br>"
        "Webpage: <a href=\"https://%5/\">%5</a> <br>"
        "Source: <a href=\"https://%2/\">%2</a> <br> <br>"
        "%3 Core %4 <br> <br>"
        "This software is licensed under the GNU Lesser General Public License v3.0.<br>"
        "Copyright (C) %1 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)"
    ).arg(
        /*1: year*/ QStringLiteral("2023"),
        /*2: source*/ QStringLiteral("github.com/ManiVaultStudio"),
        /*3: name*/ _baseNameAction.getString(),
        /*4: version*/ QString::fromStdString(Application::current()->getVersion().getVersionString()),
        /*5: webpage*/ QStringLiteral("manivault.studio")
    ));
}

void BrandingConfigurationAction::fromVariantMap(const QVariantMap& variantMap)
{
    VerticalGroupAction::fromVariantMap(variantMap);

    _organizationNameAction.fromParentVariantMap(variantMap, true);
    _baseNameAction.fromParentVariantMap(variantMap, true);
    _fullNameAction.fromParentVariantMap(variantMap, true);
    _editFullNameAction.fromParentVariantMap(variantMap, true);
    _logoAction.fromParentVariantMap(variantMap, true);
	_iconAction.fromParentVariantMap(variantMap, true);
    _splashScreenAction.fromParentVariantMap(variantMap, true);
    _aboutAction.fromParentVariantMap(variantMap, true);
}

QVariantMap BrandingConfigurationAction::toVariantMap() const
{
    auto variantMap = VerticalGroupAction::toVariantMap();

    _organizationNameAction.insertIntoVariantMap(variantMap);
    _baseNameAction.insertIntoVariantMap(variantMap);
    _fullNameAction.insertIntoVariantMap(variantMap);
    _editFullNameAction.insertIntoVariantMap(variantMap);
    _logoAction.insertIntoVariantMap(variantMap);
    _iconAction.insertIntoVariantMap(variantMap);
    _splashScreenAction.insertIntoVariantMap(variantMap);
    _aboutAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
