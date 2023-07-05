// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Version.h"

namespace hdps::util {

Version::Version(std::int32_t major, std::int32_t minor) :
    QPair<std::int32_t, std::int32_t>({ major, minor }),
    Serializable("Version")
{
}

std::int32_t Version::getMajor() const
{
    return first;
}

void Version::setMajor(std::int32_t major)
{
    first = major;
}

std::int32_t Version::getMinor() const
{
    return second;
}

void Version::setMinor(std::int32_t minor)
{
    second = minor;
}

void Version::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Major");
    variantMapMustContain(variantMap, "Minor");

    setMajor(variantMap["Major"].toInt());
    setMinor(variantMap["Minor"].toInt());
}

QVariantMap Version::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    variantMap.insert({
        { "Major", QVariant::fromValue(getMajor()) },
        { "Minor", QVariant::fromValue(getMinor()) }
    });

    return variantMap;
}

}
