// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "VersionAction.h"

#include <QStringListModel>

namespace mv::gui {

VersionAction::VersionAction(QObject* parent, const QString& title) :
    HorizontalGroupAction(parent, title),
    _majorAction(this, "Major Version", 0, 100, 1),
    _minorAction(this, "Minor Version", 0, 100, 0),
    _patchAction(this, "Patch Version", 0, 100, 0),
    _suffixAction(this, "Suffix"),
    _versionStringAction(this, "Version String")
{
    setShowLabels(false);

    addAction(&_majorAction);
    addAction(&_minorAction);
    addAction(&_patchAction);
    addAction(&_suffixAction);

    _majorAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _minorAction.setDefaultWidgetFlags(IntegralAction::SpinBox);

    _majorAction.setPrefix("major: ");
    _majorAction.setToolTip("Major version number");
    _majorAction.setStretch(1);

    _minorAction.setPrefix("minor: ");
    _minorAction.setToolTip("Minor version number");
    _minorAction.setStretch(1);

    _patchAction.setPrefix("patch: ");
    _patchAction.setToolTip("Patch version number");
    _patchAction.setStretch(1);
    _patchAction.setVisible(false);

    _suffixAction.setPlaceHolderString("Enter suffix here...");
    _suffixAction.setClearable(true);
    _suffixAction.setStretch(3);
    _suffixAction.setCompleter(&_suffixCompleter);

    _suffixCompleter.setModel(new QStringListModel({ "Alpha", "Beta", "Release Candidate", "Pre-release", "alpha", "beta", "rc", "pre-release" }));

    const auto versionElementChanged = [this]() -> void {
		_versionStringAction.setString(QString("%1.%2 %3").arg(QString::number(_majorAction.getValue()), QString::number(_minorAction.getValue()), _suffixAction.getString()));

        emit versionChanged(getVersion());
    };

    versionElementChanged();
    
    connect(&_majorAction, &IntegralAction::valueChanged, this, versionElementChanged);
    connect(&_minorAction, &IntegralAction::valueChanged, this, versionElementChanged);
    connect(&_patchAction, &IntegralAction::valueChanged, this, versionElementChanged);
    connect(&_suffixAction, &StringAction::stringChanged, this, versionElementChanged);
}

std::int32_t VersionAction::getMajor() const
{
    return getMajorAction().getValue();
}

std::int32_t VersionAction::getMinor() const
{
    return getMinorAction().getValue();
}

std::int32_t VersionAction::getPatch() const
{
    return getPatchAction().getValue();
}

QString VersionAction::getSuffix() const
{
    return getSuffixAction().getString();
}

util::Version VersionAction::getVersion() const
{
    return { getMajor(), getMinor(), getPatch(), getSuffix().toStdString() };
}

void VersionAction::setVersion(const util::Version& version)
{
    getMajorAction().setValue(version.getMajor());
    getMinorAction().setValue(version.getMinor());
    getPatchAction().setValue(version.getPatch());
    getSuffixAction().setString(QString::fromStdString(version.getSuffix()));
}

void VersionAction::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    _majorAction.fromParentVariantMap(variantMap);
    _minorAction.fromParentVariantMap(variantMap);
    _suffixAction.fromParentVariantMap(variantMap);
}

QVariantMap VersionAction::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    _majorAction.insertIntoVariantMap(variantMap);
    _minorAction.insertIntoVariantMap(variantMap);
    _patchAction.insertIntoVariantMap(variantMap);
    _suffixAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
