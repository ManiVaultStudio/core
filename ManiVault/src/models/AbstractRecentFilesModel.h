// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "models/StandardItemModel.h"

#include "util/RecentFile.h"

namespace mv {

/**
 * Abstract recent files model class
 *
 * Standard item model storing and manipulating recent file paths.
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractRecentFilesModel : public StandardItemModel
{
    Q_OBJECT

public:

    /** Model columns */
    enum class Column {
        FilePath,       /** Location of the recent file */
        DateTime        /** Date and time when the file was opened */
    };

    /** Base standard model item class for recent file */
    class CORE_EXPORT Item : public QStandardItem {
    public:

        /**
         * Construct with \p recentFile
         * @param recentFile Recent file object containing file path and date time
         */
        Item(const util::RecentFile& recentFile);

        /**
         * Get recent file
         * return Recent file object
         */
        util::RecentFile getRecentFile() const {
            return _recentFile;
        }

    private:
        util::RecentFile    _recentFile;    /** The recent file */
    };

protected:

    /** Standard model item class for displaying the recent file path */
    class FilePathItem final : public Item {
    public:

        /** No need for custom constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /**
         * Get header data for \p orientation and \p role
         * @param orientation Horizontal/vertical
         * @param role Data role
         * @return Header data
         */
        static QVariant headerData(Qt::Orientation orientation, int role) {
            switch (role) {
	            case Qt::DisplayRole:
	            case Qt::EditRole:
	                return "File path";

	            case Qt::ToolTipRole:
	                return "Recent file path";

	            default:
	                break;
            }

            return {};
        }
    };

    /** Standard model item class for displaying the recent file date and time */
    class DateTimeItem final : public Item {
    public:

        /** No need for custom constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /**
         * Get header data for \p orientation and \p role
         * @param orientation Horizontal/vertical
         * @param role Data role
         * @return Header data
         */
        static QVariant headerData(Qt::Orientation orientation, int role) {
            switch (role) {
	            case Qt::DisplayRole:
	            case Qt::EditRole:
	                return "Date & time";

	            case Qt::ToolTipRole:
	                return "Recent file date and time";

	            default:
	                break;
            }

            return {};
        }
    };

    /** Convenience class for combining items in a row */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct with \p recentFile object
         * @param recentFile Recent file object containing file path and date time
         */
        Row(const util::RecentFile& recentFile) :
            QList<QStandardItem*>()
        {
            append(new FilePathItem(recentFile));
            append(new DateTimeItem(recentFile));
        }
    };

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     * @param recentFileType Type of recent file, e.g. "Project" or "Workspace"
     */
    AbstractRecentFilesModel(QObject* parent, const QString& recentFileType);

    /**
     * Get header data for \p section, \p orientation and display \p role
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * Get recent file object from \p index
     * @param index Model index
     * @return Item at \p index
     */
    util::RecentFile getRecentFileFromIndex(const QModelIndex& index) const;

    /**
     * Get recent files
     * @return List of recent file objects
     */
    virtual util::RecentFiles getRecentFiles() const = 0;

    /**
     * Get recent file type
     * @return Recent file type, e.g. "Project" or "Workspace"
     */
    QString getRecentFileType() const {
        return _recentFileType;
    }

    /**
     * Get settings key (Models/<serialization_name>/<recent_file_type>)
     * @return Settings key
     */
    QString getSettingsKey() const;

signals:

    /** Signals that the model has been re-populated */
    void populated();

private:
    QString   _recentFileType;    /** Type of recent file, e.g. "Project" or "Workspace" */
};

}
