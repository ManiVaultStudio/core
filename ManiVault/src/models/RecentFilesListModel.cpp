// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "RecentFilesListModel.h"

#ifdef _DEBUG
    #define RECENT_FILES_LIST_MODEL_VERBOSE
#endif

using namespace mv::util;

namespace mv {

RecentFilesListModel::RecentFilesListModel(QObject* parent, const QString& recentFileType) :
    AbstractRecentFilesModel(parent, recentFileType)
{
    setColumnCount(2);

    load();
}

void RecentFilesListModel::load()
{
    setRowCount(0);

    const auto recentFiles = Application::current()->getSetting(getSettingsKey(), QVariantList()).toList();

    for (const auto& recentFile : recentFiles) {
        const auto filePath = recentFile.toMap()["FilePath"].toString();
        const auto dateTime = recentFile.toMap()["DateTime"].toDateTime();

        if (!QFileInfo(filePath).exists())
            continue;

        appendRow(Row(RecentFile(filePath, dateTime)));
    }

    if (rowCount() > maxRecentFiles)
        setRowCount(maxRecentFiles);

    emit populated();
}

void RecentFilesListModel::save() const
{
    QVariantList recentFileMaps;

    for (const auto& recentFile : getRecentFiles()) {
        QVariantMap recentFileMap{
	        { "FilePath", recentFile.getFilePath() },
	        { "DateTime", recentFile.getDateTime() }
	    };

        recentFileMaps << recentFileMap;
    }

    Application::current()->setSetting(getSettingsKey(), recentFileMaps);
}

void RecentFilesListModel::addRecentFilePath(const QString& filePath)
{
    removeRecentFilePath(filePath);
    appendRow(Row(RecentFile(filePath, QDateTime::currentDateTime())));

	while (rowCount() > maxRecentFiles)
        removeRow(rowCount() - 1);

	save();

    emit populated();
}

void RecentFilesListModel::removeRecentFilePath(const QString& filePath)
{
    const auto matches = match(index(0, static_cast<std::int32_t>(Column::FilePath)), Qt::DisplayRole, filePath, -1, Qt::MatchExactly | Qt::MatchRecursive);

	for (const auto& match : matches)
        if (match.isValid())
            removeRow(match.row(), match.parent());

    save();

    emit populated();
}

QStringList RecentFilesListModel::getRecentFilePaths() const
{
    QStringList recentFilePaths;

    for (int rowIndex = 0; rowIndex < rowCount(); rowIndex++)
        recentFilePaths << data(index(rowIndex, static_cast<int>(Column::FilePath))).toString();

    return recentFilePaths;
}

RecentFiles RecentFilesListModel::getRecentFiles() const
{
    RecentFiles recentFiles;

    for (int rowIndex = 0; rowIndex < rowCount(); rowIndex++) {
        const auto filePath = data(index(rowIndex, static_cast<int>(Column::FilePath))).toString();
        const auto dateTime = data(index(rowIndex, static_cast<int>(Column::DateTime))).toDateTime();

    	if (!filePath.isEmpty())
            recentFiles.emplace_back(RecentFile(filePath, dateTime));
    }

    return recentFiles;
}

}
