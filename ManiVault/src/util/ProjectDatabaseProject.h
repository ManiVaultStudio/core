// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "Version.h"

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
class CORE_EXPORT ProjectDatabaseProject : public QObject
{
    Q_OBJECT

public:
    
    /**
     * Construct project from \p variantMap
     * @param variantMap Variant map containing the project properties
     */
    explicit ProjectDatabaseProject(const QVariantMap& variantMap);

    /**
     * Get title
     * @return Project title
     */
    const QString& getTitle() const;

    /**
     * Get group
     * @return Project group
     */
    const QString& getGroup() const;

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
     * Get minimum supported ManiVault Studio core version
     * @return Minimum supported ManiVault Studio core  version
     */
    const Version& getMinimumCoreVersion() const;

    /**
     * Get required plugins
     * @return Required plugins
     */
    const QStringList& getRequiredPlugins() const;

    /**
     * Get missing plugins
     * @return Missing plugins
     */
    const QStringList& getMissingPlugins() const;

    /**
     * Overload assignment operator
     * @param rhs Right hand side project
     * @return Assigned project
     */
    ProjectDatabaseProject& operator=(const ProjectDatabaseProject& rhs)
    {
        _title                  = rhs.getTitle();
        _group                  = rhs.getTitle();
        _tags                   = rhs.getTags();
        _date                   = rhs.getDate();
        _iconName               = rhs.getIconName();
        _summary                = rhs.getSummary();
        _url                    = rhs.getUrl();
        _minimumCoreVersion     = rhs.getMinimumCoreVersion();
        _requiredPlugins        = rhs.getRequiredPlugins();
        _missingPlugins         = rhs.getMissingPlugins();

        return *this;
    }

private:
    QString         _title;                 /** Title */
    QString         _group;                 /** Group */
    QStringList     _tags;                  /** Tags */
    QString         _date;                  /** Issue date */
    QString         _iconName;              /** Font Awesome icon name */
    QString         _summary;               /** Summary (brief description) */
    QUrl            _url;                   /** Project URL */
    Version         _minimumCoreVersion;    /** Minimum supported ManiVault Studio major version */
    QStringList     _requiredPlugins;       /** Required plugins */
    QStringList     _missingPlugins;        /** Missing plugins */
};

using ProjectDatabaseProjects = std::vector<const ProjectDatabaseProject*>;

}
