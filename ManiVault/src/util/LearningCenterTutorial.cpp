// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningCenterTutorial.h"

using namespace mv::gui;

namespace mv::util {

LearningCenterTutorial::LearningCenterTutorial(const QString& title, const QStringList& tags, const QString& date, const QString& iconName, const QString& summary, const QString& content, const QUrl& url) :
    _title(title),
    _tags(tags),
    _date(date),
    _iconName(iconName),
    _summary(summary),
    _content(content),
    _url(url)
{
}

LearningCenterTutorial::LearningCenterTutorial(const QVariantMap& variantMap) :
    _title(variantMap["title"].toString()),
    _tags(variantMap["tags"].toStringList()),
    _date(variantMap["date"].toString()),
    _iconName(variantMap["icon"].toString()),
    _summary(variantMap["summary"].toString()),
    _content(variantMap["fullpost"].toString()),
    _url(QUrl(variantMap["url"].toString()))
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

}
