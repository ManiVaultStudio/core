// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningCenterTutorial.h"

namespace mv::util {

LearningCenterTutorial::LearningCenterTutorial(const QString& title, const QStringList& tags, const QString& date, const QString& iconName, const QString& summary, const QString& content, const QUrl& url, const QUrl& projectUrl, const std::int32_t minimumVersionMajor, const std::int32_t minimumVersionMinor) :
    _title(title),
    _tags(tags),
    _date(date),
    _iconName(iconName),
    _summary(summary),
    _content(content),
    _url(url),
    _projectUrl(projectUrl),
    _minimumVersionMajor(minimumVersionMajor),
    _minimumVersionMinor(minimumVersionMinor)
{
}

LearningCenterTutorial::LearningCenterTutorial(const QVariantMap& variantMap) :
    _title(variantMap.contains("title") ? variantMap["title"].toString() : ""),
    _tags(variantMap.contains("tags") ? variantMap["tags"].toStringList() : QStringList()),
    _date(variantMap.contains("date") ? variantMap["date"].toString() : ""),
    _iconName(variantMap.contains("icon") ? variantMap["icon"].toString() : ""),
    _summary(variantMap.contains("summary") ? variantMap["summary"].toString() : ""),
    _content(variantMap.contains("fullpost") ? variantMap["fullpost"].toString() : ""),
    _url(QUrl(variantMap.contains("url") ? variantMap["url"].toString() : "")),
    _projectUrl(variantMap.contains("project") ? QUrl(variantMap["project"].toString()) : QUrl()),
    _minimumVersionMajor(variantMap.contains("minimum-version-major") ? variantMap["minimum-version-major"].toInt() : -1),
    _minimumVersionMinor(variantMap.contains("minimum-version-minor") ? variantMap["minimum-version-minor"].toInt() : -1)
{
}

const QString& LearningCenterTutorial::getTitle() const
{
    return _title;
}

const QStringList& LearningCenterTutorial::getTags() const
{
    return _tags;
}

const QString& LearningCenterTutorial::getDate() const
{
    return _date;
}

const QString& LearningCenterTutorial::getIconName() const
{
    return _iconName;
}

const QString& LearningCenterTutorial::getSummary() const
{
    return _summary;
}

const QString& LearningCenterTutorial::getContent() const
{
    return _content;
}

const QUrl& LearningCenterTutorial::getUrl() const
{
    return _url;
}

const QUrl& LearningCenterTutorial::getProjectUrl() const
{
    return _projectUrl;
}

const std::int32_t& LearningCenterTutorial::getMinimumVersionMajor() const
{
    return _minimumVersionMajor;
}

const std::int32_t& LearningCenterTutorial::getMinimumVersionMinor() const
{
    return _minimumVersionMinor;
}

bool LearningCenterTutorial::hasProject() const
{
    return !_projectUrl.isEmpty();
}

}
