// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <models/StandardItemModel.h>
#include <ProjectMetaAction.h>

#include <QList>
#include <QStandardItem>

/**
 * IsStartup projects model class
 *
 * Standard item model class for storing startup projects
 *
 * @author Thomas Kroes
 */
class StartupProjectsModel : public mv::StandardItemModel
{
public:

    /** Model columns */
    enum class Column {
        FileName,       /** File name of the project */
        Title,          /** Tile of the project */
        Description,    /** Description of the project */

        Count
    };

    /** Standard model item class for displaying the startup project file name */
    class FileNameItem final : public QStandardItem {
    public:

        /** No need for specialized constructor */
        using QStandardItem::QStandardItem;

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
                    return "File name";

                case Qt::ToolTipRole:
                    return "IsStartup project file name";
            }

            return {};
        }
    };

    /** Standard model item class for displaying the startup project title */
    class TitleItem final : public QStandardItem {
    public:

        /** No need for specialized constructor */
        using QStandardItem::QStandardItem;

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
                    return "Title";

                case Qt::ToolTipRole:
                    return "IsStartup project title";
            }

            return {};
        }
    };

    /** Standard model item class for displaying the startup project description */
    class DescriptionItem final : public QStandardItem {
    public:

        /** No need for specialized constructor */
        using QStandardItem::QStandardItem;

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
                    return "Description";

                case Qt::ToolTipRole:
                    return "IsStartup project description";
            }

            return {};
        }
    };

protected:

    /** Convenience class for combining items in a row */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct with \p icon, \p title and \p description
         * @param icon Project icon
         * @param fileName Project file name
         * @param title Project title
         * @param description Project description
         */
        Row(const QIcon icon, const QString& fileName, const QString& title, const QString& description) : QList<QStandardItem*>()
        {
            auto fileNameItem       = new FileNameItem(fileName);
            auto titleItem          = new TitleItem(title);
            auto descriptionItem    = new DescriptionItem(description);

#ifdef _DEBUG
            fileNameItem->setIcon(icon);
#else
            titleItem->setIcon(icon);
#endif

            append(fileNameItem);
            append(titleItem);
            append(descriptionItem);
        }
    };

public:

    /**
     * Construct startup projects model with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    StartupProjectsModel(QObject* parent = nullptr);

    /**
     * Get header data for \p section, \p orientation and display \p role
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * Initialize the model from \p startupProjectsMetaActions
     * @param startupProjectsMetaActions Startup projects meta actions
     */
    void initialize(const QVector<QPair<QSharedPointer<mv::ProjectMetaAction>, QString>>& startupProjectsMetaActions);
};
