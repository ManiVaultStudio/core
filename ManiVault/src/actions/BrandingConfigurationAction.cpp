// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "BrandingConfigurationAction.h"
#include "Application.h"
#include "CoreInterface.h"

#include "util/Icon.h"
#include "util/StyledIcon.h"

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
    _generateIconFromLogoAction(this, "Generate icon from logo"),
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
    _generateIconFromLogoAction.setToolTip("Generate the application icon from the current logo");
    _splashScreenAction.setToolTip("Configure the splash screen");
    _aboutAction.setToolTip("Overrides the default (HTML) text in the about dialog");

    const auto nonEmptyValidator = QRegularExpression("^.+$");

    _organizationNameAction.getValidator().setRegularExpression(nonEmptyValidator);
    _organizationDomainAction.getValidator().setRegularExpression(nonEmptyValidator);
    _baseNameAction.getValidator().setRegularExpression(nonEmptyValidator);

    _logoAction.setDefaultWidgetFlags(ImageAction::WidgetFlag::Loader);

    _editFullNameAction.setChecked(false);

    _iconAction.getIconPickerAction().setIcon(createIcon(QPixmap(":/Icons/AppIcon256")));

    _aboutAction.setDefaultWidgetFlags(StringAction::WidgetFlag::TextEdit);

    addAction(&_organizationNameAction);
    addAction(&_organizationDomainAction);
    addAction(&_baseNameAction);
    addAction(&_fullNameAction);
    addAction(&_editFullNameAction);
    addAction(&_logoAction);
    addAction(&_iconAction);
    addAction(&_generateIconFromLogoAction);
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

    const auto updateFromLogoReadOnly = [this]() -> void {
        _generateIconFromLogoAction.setEnabled(!_logoAction.getImage().isNull());
	};

    updateFromLogoReadOnly();

    connect(&_logoAction, &ImageAction::imageChanged, this, updateFromLogoReadOnly);

    connect(&_generateIconFromLogoAction, &TriggerAction::triggered, this, [this]() -> void {
        if (_logoAction.getImage().isNull())
            return;

        const auto logoPixmap = QPixmap::fromImage(_logoAction.getImage());

    	if (logoPixmap.isNull())
            return;

        if (logoPixmap.size().width() != logoPixmap.size().height())
            mv::help().addNotification("Logo image is not a square", "The logo is not square and will be resized to a square to fit.", StyledIcon("palette"));

        const auto scaledLogoPixmap = logoPixmap.scaled(QSize(64, 64), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        _iconAction.getIconPickerAction().setIcon(QIcon(scaledLogoPixmap));
    });

    auto setAboutString = [this]() {
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
        };

    connect(&_baseNameAction, &StringAction::stringChanged, this, setAboutString);

    setAboutString();
}

void BrandingConfigurationAction::fromVariantMap(const QVariantMap& variantMap)
{
    VerticalGroupAction::fromVariantMap(variantMap);

    _organizationNameAction.fromParentVariantMap(variantMap, true);
    _organizationDomainAction.fromParentVariantMap(variantMap, true);
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
    _organizationDomainAction.insertIntoVariantMap(variantMap);
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
