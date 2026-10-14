// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractRecentFilesModel.h"

#ifdef _DEBUG
    #define ABSTRACT_RECENT_FILES_MODEL_VERBOSE
#endif

using namespace mv::util;

namespace mv {

AbstractRecentFilesModel::Item::Item(const util::RecentFile& recentFile) :
    _recentFile(recentFile)
{
    setEditable(false);
}

QVariant AbstractRecentFilesModel::FilePathItem::data(int role) const {
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getRecentFile().getFilePath();

        case Qt::ToolTipRole:
            return "File path: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractRecentFilesModel::DateTimeItem::data(int role) const {
    switch (role) {
        case Qt::EditRole:
            return getRecentFile().getDateTime();

        case Qt::DisplayRole:
            return data(Qt::EditRole).toDateTime().toString();

        case Qt::ToolTipRole:
            return "Date & time: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

AbstractRecentFilesModel::AbstractRecentFilesModel(QObject* parent, const QString& recentFileType) :
    StandardItemModel(parent, "RecentFiles"),
    _recentFileType(recentFileType)
{
}

QVariant AbstractRecentFilesModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    switch (static_cast<Column>(section))
    {
        case Column::FilePath:
            return FilePathItem::headerData(orientation, role);

        case Column::DateTime:
            return DateTimeItem::headerData(orientation, role);
    }

    return {};
}

RecentFile AbstractRecentFilesModel::getRecentFileFromIndex(const QModelIndex& index) const
{
    if (!index.isValid())
        return {};

    if (const auto item = itemFromIndex(index))
        return dynamic_cast<Item*>(item)->getRecentFile();

    return {};
}

QString AbstractRecentFilesModel::getSettingsKey() const
{
    return QString("%1/%2").arg(StandardItemModel::getSettingsPrefix(), getRecentFileType());
}

}
