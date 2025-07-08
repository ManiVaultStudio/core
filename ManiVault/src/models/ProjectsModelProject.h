// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "util/HardwareSpec.h"
#include "util/Version.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>

#include <vector>

namespace mv::util {

/**
 * Projects model project class
 *
 * Contains project information which is used in the project models
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ProjectsModelProject : public QObject
{
    Q_OBJECT

public:
    
    /**
     * Construct project from \p variantMap
     * @param variantMap Variant map containing the project properties
     */
    explicit ProjectsModelProject(const QVariantMap& variantMap);

    /**
     * Construct project group from \p groupTitle
     * @param groupTitle Project group title
     */
    explicit ProjectsModelProject(const QString& groupTitle);

    /** Load the project with the project manager */
    void load() const;

    /**
     * Get title
     * @return Project title
     */
    QString getTitle() const;

    /**
     * Get whether project has been downloaded before
     * @return Boolean determining whether project has been downloaded before
     */
    bool isDownloaded() const;

    /**
     * Get whether this is a group
     * @return Boolean determining whether this is a group or a project
     */
    bool isGroup() const;

    /**
     * Get group
     * @return Project group
     */
    QString getGroup() const;

    /**
     * Get tags
     * @return Project tags
     */
    QStringList getTags() const;

    /**
     * Get date
     * @return Issue date 
     */
    QString getDate() const;

    /**
     * Get icon name
     * @return Font Awesome icon name
     */
    QString getIconName() const;

    /**
     * Get summary
     * @return Project summary (brief description)
     */
    QString getSummary() const;

    /**
     * Get URL
     * @return ManiVault website project URL
     */
    QUrl getUrl() const;

    /**
     * Get minimum supported ManiVault Studio core version
     * @return Minimum supported ManiVault Studio core  version
     */
    Version getMinimumCoreVersion() const;

    /**
     * Get required plugins
     * @return Required plugins
     */
    QStringList getRequiredPlugins() const;

    /**
     * Get missing plugins
     * @return Missing plugins
     */
    QStringList getMissingPlugins() const;

    /**
     * Get size
     * @return Project size as a string (e.g., "1.2 MB", "500 KB", etc.)
     */
    QString getSize() const;

    /**
     * Get minimum hardware specification
     * @return Project minimum hardware specification
     */
    HardwareSpec getMinimumHardwareSpec() const;

    /**
     * Get recommended hardware specification
     * @return Project recommended hardware specification
     */
    HardwareSpec getRecommendedHardwareSpec() const;

    /**
     * Get whether this is a startup project
     * @return Boolean determining whether this is a startup project
     */
    bool isStartup() const;

    /**
     * Overload assignment operator
     * @param rhs Right hand side project
     * @return Assigned project
     */
    ProjectsModelProject& operator=(const ProjectsModelProject& rhs)
    {
        _title                      = rhs.getTitle();
        _group                      = rhs.getGroup();
    	_isGroup                    = rhs.isGroup();
        _tags                       = rhs.getTags();
        _date                       = rhs.getDate();
        _iconName                   = rhs.getIconName();
        _summary                    = rhs.getSummary();
        _url                        = rhs.getUrl();
        _minimumCoreVersion         = rhs.getMinimumCoreVersion();
        _requiredPlugins            = rhs.getRequiredPlugins();
        _missingPlugins             = rhs.getMissingPlugins();
        _size                       = rhs.getSize();
        _minimumHardwareSpec        = rhs.getMinimumHardwareSpec();
        _recommendedHardwareSpec    = rhs.getRecommendedHardwareSpec();
        _startup                  = rhs.isStartup();

        return *this;
    }

private:
    QString         _title;                     /** Title */
    bool            _isGroup;                   /** Boolean determining whether this is a group or a project */
    QString         _group;                     /** Group */
    QStringList     _tags;                      /** Tags */
    QString         _date;                      /** Issue date */
    QString         _iconName;                  /** Font Awesome icon name */
    QString         _summary;                   /** Summary (brief description) */
    QUrl            _url;                       /** Project URL */
    Version         _minimumCoreVersion;        /** Minimum supported ManiVault Studio major version */
    QStringList     _requiredPlugins;           /** Required plugins */
    QStringList     _missingPlugins;            /** Missing plugins */
    QString         _size;                      /** Size of the ManiVault project */
    HardwareSpec    _minimumHardwareSpec;       /** Minimum hardware specification for the project */
    HardwareSpec    _recommendedHardwareSpec;   /** Recommended hardware specification for the project */
    bool            _startup;                 /** Boolean determining whether this is a startup project */
};

using ProjectDatabaseProjects = std::vector<const ProjectsModelProject*>;

}
