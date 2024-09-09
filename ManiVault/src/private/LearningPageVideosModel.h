// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/FileDownloader.h>

#include <QMap>
#include <QStandardItemModel>
#include <QJsonObject>

/**
 * Learning page videos model class
 *
 * Model class which contains learning page video content
 *
 * @author Thomas Kroes
 */
class LearningPageVideosModel final : public QStandardItemModel
{
    Q_OBJECT

public:

    /** Model columns */
    enum class Column {
        Title,
        Tags,
        Date,
        Summary,
        YouTubeId,
        YouTubeUrl,
        Delegate,

        Count
    };

    /** Header strings for several data roles */
    struct ColumHeaderInfo {
        QString     _display;   /** Header string for display role */
        QString     _edit;      /** Header string for edit role */
        QString     _tooltip;   /** Header string for tooltip role */
    };

    /** Column name and tooltip */
    static QMap<Column, ColumHeaderInfo> columnInfo;

protected:

    /** Base standard model item class for video */
    class Item : public QStandardItem {
    public:

        /**
         * Construct with \p variantMap of video
         * @param variantMap Variant map describing the video
         */
        Item(QVariantMap variantMap, bool editable = false);

        /**
         * Get variant map
         * return Const reference to the JSON object
         */
        const QVariantMap& getVariantMap() const;

    private:
        QVariantMap _variantMap;      /** The video JSON object */
    };

    /** Standard model item class for displaying the video title */
    class TitleItem final : public Item {
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
                    return "Title";

                case Qt::ToolTipRole:
                    return "Video title";
            }

            return {};
        }
    };

    /** Standard model item class for displaying the video tags */
    class TagsItem final : public Item {
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
                    return "Tags";

                case Qt::ToolTipRole:
                    return "Video tags";
            }

            return {};
        }
    };

    /** Standard model item class for displaying the video date */
    class DateItem final : public Item {
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
                    return "Date";

                case Qt::ToolTipRole:
                    return "Date at which the video was published";
            }

            return {};
        }
    };

    /** Standard model item class for displaying the video summary */
    class SummaryItem final : public Item {
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
                    return "Summary";

                case Qt::ToolTipRole:
                    return "Video description";
            }

            return {};
        }
    };

    /** Standard model item class for displaying the video YouTube identifier */
    class YouTubeIdItem final : public Item {
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
                    return "ID";

                case Qt::ToolTipRole:
                    return "YouTube identifier";
            }

            return {};
        }
    };

    /** Standard model item class for displaying the video YouTube URL */
    class YouTubeUrlItem final : public Item {
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
                    return "URL";

                case Qt::ToolTipRole:
                    return "YouTube URL";
            }

            return {};
        }
    };

    /** Standard model item class for displaying the video delegate */
    class DelegateItem final : public Item {
    public:

        /** No need for custom constructor */
        using Item::Item;

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
                    return "Delegate";

                case Qt::ToolTipRole:
                    return "Video delegate item";
            }

            return {};
        }
    };

    /** Convenience class for combining items in a row */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct with \p variantMap of video
         * @param variantMap Variant map describing the video
         */
        Row(QVariantMap variantMap) :
            QList<QStandardItem*>()
        {
            append(new TitleItem(variantMap));
            append(new TagsItem(variantMap));
            append(new DateItem(variantMap));
            append(new SummaryItem(variantMap));
            append(new YouTubeIdItem(variantMap));
            append(new YouTubeUrlItem(variantMap));
            append(new DelegateItem(variantMap));
        }
    };

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    LearningPageVideosModel(QObject* parent = nullptr);

    /**
     * Get header data for \p section, \p orientation and display \p role
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * Get tags
     * @return All tags
     */
    QSet<QString> getTagsSet() const;

    /** Loads learning center JSON file and picks out the videos */
    void populateFromServer();

signals:

    /**
     * Signals that the tags changed to \p tags
     * @param tags New tags
     */
    void tagsChanged(const QSet<QString>& tags);

private:
    mv::util::FileDownloader    _fileDownloader;    /** For downloading the learning center JSON file */
    QSet<QString>               _tags;              /** All tags */
};
