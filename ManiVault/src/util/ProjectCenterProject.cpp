// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectCenterProject.h"

#include <QVariantMap>

namespace mv::util {

ProjectCenterProject::ProjectCenterProject(const QString& title, const QStringList& tags, const QString& date, const QString& iconName, const QString& summary, const QUrl& url, const std::int32_t minimumVersionMajor, const std::int32_t minimumVersionMinor) :
    _title(title),
    _tags(tags),
    _date(date),
    _iconName(iconName),
    _summary(summary),
    _url(url),
    _minimumVersionMajor(minimumVersionMajor),
    _minimumVersionMinor(minimumVersionMinor)
{
}

ProjectCenterProject::ProjectCenterProject(const QVariantMap& variantMap) :
    _title(variantMap.contains("projectName") ? variantMap["projectName"].toString() : ""),
    _tags(variantMap.contains("tags") ? variantMap["tags"].toStringList() : QStringList()),
    _date(variantMap.contains("date") ? variantMap["date"].toString() : ""),
    _iconName(variantMap.contains("icon") ? variantMap["icon"].toString() : ""),
    _summary(variantMap.contains("projectComments") ? variantMap["projectComments"].toString() : ""),
    _url(QUrl(variantMap.contains("projectUrl") ? variantMap["projectUrl"].toString() : "")),
    _minimumVersionMajor(variantMap.contains("minimum-version-major") ? variantMap["minimum-version-major"].toInt() : -1),
    _minimumVersionMinor(variantMap.contains("minimum-version-minor") ? variantMap["minimum-version-minor"].toInt() : -1)
{
}

const QString& ProjectCenterProject::getTitle() const
{
    return _title;
}

const QStringList& ProjectCenterProject::getTags() const
{
    return _tags;
}

const QString& ProjectCenterProject::getDate() const
{
    return _date;
}

const QString& ProjectCenterProject::getIconName() const
{
    return _iconName;
}

const QString& ProjectCenterProject::getSummary() const
{
    return _summary;
}

const QUrl& ProjectCenterProject::getUrl() const
{
    return _url;
}

const std::int32_t& ProjectCenterProject::getMinimumVersionMajor() const
{
    return _minimumVersionMajor;
}

const std::int32_t& ProjectCenterProject::getMinimumVersionMinor() const
{
    return _minimumVersionMinor;
}

}
