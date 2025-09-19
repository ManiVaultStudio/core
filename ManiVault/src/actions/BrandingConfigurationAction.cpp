// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "BrandingConfigurationAction.h"

using namespace mv::util;

namespace mv::gui {

BrandingConfigurationAction::BrandingConfigurationAction(QObject* parent, const QString& title) :
    VerticalGroupAction(parent, title),
    _baseNameAction(this, "Base name"),
    _fullNameAction(this, "Full name"),
    _editFullNameAction(this, "Edit full name"),
    _logoAction(this, "Logo"),
    _applicationIconAction(this, "Application icon"),
	_iconFromLogoAction(this, "Generate icon from logo")
{
    _logoAction.setDefaultWidgetFlags(ImageAction::WidgetFlag::Loader);

    _editFullNameAction.setChecked(false);

    addAction(&_baseNameAction);
    addAction(&_fullNameAction);
    addAction(&_editFullNameAction);
    addAction(&_logoAction);
    addAction(&_applicationIconAction);
    addAction(&_iconFromLogoAction);

    const auto updateFullNameAction = [this]() -> void {
        _fullNameAction.setEnabled(_editFullNameAction.isChecked());

        if (!_editFullNameAction.isChecked())
            _fullNameAction.setString(QString("%1 %2").arg(_baseNameAction.getString(), QString::fromStdString(Application::current()->getVersion().getVersionString())));
    };

    updateFullNameAction();

    connect(&_editFullNameAction, &ToggleAction::toggled, this, updateFullNameAction);
    connect(&_baseNameAction, &StringAction::stringChanged, this, updateFullNameAction);

    const auto updateApplicationIconAction = [this]() -> void {
        _applicationIconAction.setEnabled(_iconFromLogoAction.isChecked());
	};

    updateApplicationIconAction();

    connect(&_iconFromLogoAction, &ToggleAction::toggled, this, updateApplicationIconAction);
}

void BrandingConfigurationAction::fromVariantMap(const QVariantMap& variantMap)
{
    VerticalGroupAction::fromVariantMap(variantMap);

    _baseNameAction.fromParentVariantMap(variantMap, true);
    _fullNameAction.fromParentVariantMap(variantMap, true);
    _editFullNameAction.fromParentVariantMap(variantMap, true);
    _applicationIconAction.fromParentVariantMap(variantMap, true);
    _logoAction.fromParentVariantMap(variantMap, true);
}

QVariantMap BrandingConfigurationAction::toVariantMap() const
{
    auto variantMap = VerticalGroupAction::toVariantMap();

    _baseNameAction.insertIntoVariantMap(variantMap);
    _fullNameAction.insertIntoVariantMap(variantMap);
    _editFullNameAction.insertIntoVariantMap(variantMap);
    _applicationIconAction.insertIntoVariantMap(variantMap);
    _logoAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
