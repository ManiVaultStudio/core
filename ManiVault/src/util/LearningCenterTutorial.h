// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>

#include <vector>

namespace mv::util {

/**
 * LearningCenterTutorial class
 *
 * Contains tutorial information
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT LearningCenterTutorial : public QObject
{
    Q_OBJECT

public:
    
    /**
     * Construct tutorial from individual properties
     * @param title Tutorial title
     * @param tags Tutorial tags for filtering
     * @param date Issue date
     * @param iconName Font Awesome icon name
     * @param summary Tutorial summary (brief description)
     * @param content Full tutorial content in HTML format
     * @param url ManiVault website tutorial URL
     * @param projectUrl Location of the ManiVault tutorial project (if any)
     * @param minimumVersionMajor Minimum supported ManiVault Studio major version
     * @param minimumVersionMinor Minimum supported ManiVault Studio minor version
     */
    explicit LearningCenterTutorial(const QString& title, const QStringList& tags, const QString& date, const QString& iconName, const QString& summary, const QString& content, const QUrl& url, const QUrl& projectUrl, const std::int32_t minimumVersionMajor, const std::int32_t minimumVersionMinor);

    /**
     * Construct tutorial from \p variantMap
     * @param variantMap Variant map containing the tutorial properties
     */
    explicit LearningCenterTutorial(const QVariantMap& variantMap);

    /**
     * Get title
     * @return Tutorial title
     */
    const QString& getTitle() const;

    /**
     * Get tags
     * @return Tutorial tags
     */
    const QStringList& getTags() const;

    /**
     * Get date
     * @return Issue date 
     */
    const QString& getDate() const;

    /**
     * Get icon name
     * @return Font Awesome icon name
     */
    const QString& getIconName() const;

    /**
     * Get summary
     * @return Tutorial summary (brief description)
     */
    const QString& getSummary() const;

    /**
     * Get content
     * @return Full tutorial content in HTML format
     */
    const QString& getContent() const;

    /**
     * Get URL
     * @return ManiVault website tutorial URL
     */
    const QUrl& getUrl() const;

    /**
     * Get project URL
     * @return Location of the ManiVault tutorial project (if any)
     */
    const QUrl& getProjectUrl() const;

    /**
     * Get minimum supported ManiVault Studio major version
     * @return Minimum supported ManiVault Studio major version
     */
    const std::int32_t& getMinimumVersionMajor() const;

    /**
     * Get minimum supported ManiVault Studio minor version
     * @return Minimum supported ManiVault Studio minor version
     */
    const std::int32_t& getMinimumVersionMinor() const;

    /**
     * Get whether the tutorial has a project
     * @return Boolean determining whether the tutorial has a project
     */
    bool hasProject() const;

    /**
     * Overload assignment operator
     * @param rhs Right hand side tutorial
     * @return Assigned tutorial
     */
    LearningCenterTutorial& operator=(const LearningCenterTutorial& rhs)
    {
        _title                  = rhs.getTitle();
        _tags                   = rhs.getTags();
        _date                   = rhs.getDate();
        _iconName               = rhs.getIconName();
        _summary                = rhs.getSummary();
        _content                = rhs.getContent();
        _url                    = rhs.getUrl();
        _projectUrl             = rhs.getProjectUrl();
        _minimumVersionMajor    = rhs.getMinimumVersionMajor();
        _minimumVersionMinor    = rhs.getMinimumVersionMinor();

        return *this;
    }

private:
    QString         _title;                 /** Title */
    QStringList     _tags;                  /** Tags */
    QString         _date;                  /** Issue date */
    QString         _iconName;              /** Font Awesome icon name */
    QString         _summary;               /** Summary (brief description) */
    QString         _content;               /** Full tutorial content in HTML format */
    QUrl            _url;                   /** ManiVault website tutorial URL */
    QUrl            _projectUrl;            /** Location of the ManiVault tutorial project (if any) */
    std::int32_t    _minimumVersionMajor;   /** Minimum supported ManiVault Studio major version */
    std::int32_t    _minimumVersionMinor;   /** Minimum supported ManiVault Studio minor version */
};

using LearningCenterTutorials = std::vector<const LearningCenterTutorial*>;

}
