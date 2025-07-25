// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractRecentFilesModel.h"

namespace mv {

/**
 * Recent files list model class
 *
 * Standard item model storing and manipulating recent file paths in list format.
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT RecentFilesListModel final : public AbstractRecentFilesModel
{
public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     * @param recentFileType Type of recent file, e.g. "Project" or "Workspace"
     */
    RecentFilesListModel(QObject* parent, const QString& recentFileType);

    /** Load recent file paths from settings */
    void load();

    /** Save to settings */
    void save() const;

    /**
     * Add recent \p filePath
     * @param filePath Recent file path to add
     */
    void addRecentFilePath(const QString& filePath);

    /**
     * Remove recent \p filePath
     * @param filePath Recent file path to remove
     */
    void removeRecentFilePath(const QString& filePath);

    /**
     * Get recent file paths
     * @return File paths as string list
     */
    QStringList getRecentFilePaths() const;

    /**
     * Get recent files
     * @return List of recent file objects
     */
    util::RecentFiles getRecentFiles() const override;

private:
    static constexpr int maxRecentFiles = 25;
};

}
