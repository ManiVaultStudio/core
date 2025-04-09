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
 * Project center project class
 *
 * Contains project information which is used in the project center
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ProjectCenterProject : public QObject
{
    Q_OBJECT

public:
    
    /**
     * Construct project center project from individual properties
     * @param title Project title
     * @param tags Project tags for filtering
     * @param date Issue date
     * @param iconName Font Awesome icon name
     * @param summary Project summary (brief description)
     * @param minimumVersionMajor Minimum supported ManiVault Studio major version
     * @param minimumVersionMinor Minimum supported ManiVault Studio minor version
     */
    explicit ProjectCenterProject(const QString& title, const QStringList& tags, const QString& date, const QString& iconName, const QString& summary, const QUrl& url, const std::int32_t minimumVersionMajor, const std::int32_t minimumVersionMinor);

    /**
     * Construct project from \p variantMap
     * @param variantMap Variant map containing the project properties
     */
    explicit ProjectCenterProject(const QVariantMap& variantMap);

    /**
     * Get title
     * @return Project title
     */
    const QString& getTitle() const;

    /**
     * Get tags
     * @return Project tags
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
     * @return Project summary (brief description)
     */
    const QString& getSummary() const;

    /**
     * Get URL
     * @return ManiVault website project URL
     */
    const QUrl& getUrl() const;

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
     * Overload assignment operator
     * @param rhs Right hand side project
     * @return Assigned project
     */
    ProjectCenterProject& operator=(const ProjectCenterProject& rhs)
    {
        _title                  = rhs.getTitle();
        _tags                   = rhs.getTags();
        _date                   = rhs.getDate();
        _iconName               = rhs.getIconName();
        _summary                = rhs.getSummary();
        _url                    = rhs.getUrl();
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
    QUrl            _url;                   /** Project URL */
    std::int32_t    _minimumVersionMajor;   /** Minimum supported ManiVault Studio major version */
    std::int32_t    _minimumVersionMinor;   /** Minimum supported ManiVault Studio minor version */
};

using ProjectCenterProjects = std::vector<const ProjectCenterProject*>;

}
