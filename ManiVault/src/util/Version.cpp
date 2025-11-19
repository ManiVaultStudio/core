// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Version.h"

namespace mv::util {

const QString Version::semanticVersioningRegex = QString(R"((0|[1-9]\d*)(?:\.(0|[1-9]\d*))?(?:\.(0|[1-9]\d*))?(?:-([\da-zA-Z-]+(?:\.[\da-zA-Z-]+)*))?(?:\+([\da-zA-Z-]+(?:\.[\da-zA-Z-]+)*))?)");

Version::Version(std::int32_t major, std::int32_t minor, std::int32_t patch, const std::string& suffix) :
    _major(major),
    _minor(minor),
    _patch(patch),
    _suffix(suffix),
    Serializable("Version")
{
    // getVersionNumber() relies on minor and patch being smaller than 100
    assert(minor < 100);
    assert(patch < 100);
}

Version::Version(const QString& version) :
    Version()
{
    initialize(version);
}

void Version::initialize(std::int32_t major, std::int32_t minor, std::int32_t patch, const std::string& suffix)
{
    setMajor(major);
    setMinor(minor);
    setPatch(patch);
    setSuffix(suffix);
}

void Version::initialize(const QString& version)
{
    QRegularExpression regex(Version::semanticVersioningRegex);

    QRegularExpressionMatch match = regex.match(version);

    if (match.hasMatch()) {
        setMajor(match.captured(1).toInt());
        setMinor(match.captured(2).toInt());
        setPatch(match.captured(3).toInt());
        setSuffix(match.captured(4).toStdString());
    }
    else {
        qWarning() << version << "is not a valid semantic version for" << QString::fromStdString(_context);
    }
}

bool Version::isValid() const
{
    if (getMajor() < 0)
        return false;

    if (getMinor() < 0)
        return false;

    if (getPatch() < 0)
        return false;

    return true;
}

std::string Version::getContext() const
{
    return _context;
}

void Version::setContext(const std::string& context)
{
    _context = context;
}

std::int32_t Version::getVersionNumber() const 
{
    return _major * 10'000 + _minor * 100 + _patch;
}

std::string Version::getVersionString() const 
{
    if (!isValid())
        return {};

    if(_suffix.empty() || _suffix.find_first_not_of(" ") == std::string::npos)
        return std::to_string(_major) + "." + std::to_string(_minor) + "." + std::to_string(_patch);
    else
        return std::to_string(_major) + "." + std::to_string(_minor) + "." + std::to_string(_patch) + "-" + _suffix;
}

std::string Version::getShortVersionString() const
{
    return std::to_string(_major) + "." + std::to_string(_minor);
}

std::int32_t Version::getMajor() const
{
    return _major;
}

void Version::setMajor(std::int32_t major)
{
    _major = major;
}

std::int32_t Version::getMinor() const
{
    return _minor;
}

void Version::setMinor(std::int32_t minor)
{
    _minor = minor;
}

std::int32_t Version::getPatch() const
{
    return _patch;
}

void Version::setPatch(std::int32_t patch)
{
    _patch = patch;
}

std::string Version::getSuffix() const
{
    return _suffix;
}

void Version::setSuffix(const std::string& suffix)
{
    _suffix = suffix;
}

void Version::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Major");
    variantMapMustContain(variantMap, "Minor");
    variantMapMustContain(variantMap, "Patch");
    variantMapMustContain(variantMap, "Suffix");

    setMajor(variantMap["Major"].toInt());
    setMinor(variantMap["Minor"].toInt());
    setPatch(variantMap["Patch"].toInt());
    setSuffix(variantMap["Suffix"].toString().toStdString());
}

QVariantMap Version::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    variantMap.insert({
        { "Major", QVariant::fromValue(getMajor()) },
        { "Minor", QVariant::fromValue(getMinor()) },
        { "Patch", QVariant::fromValue(getPatch()) },
        { "Suffix", QVariant::fromValue(getSuffix()) }
    });

    return variantMap;
}

}
