// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "util/LearningCenterVideo.h"

#include <QMap>
#include <QStandardItemModel>

namespace mv {

/**
 * Learning center videos model class
 *
 * Contains video content for the learning center
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT LearningCenterVideosModel final : public QStandardItemModel
{
    Q_OBJECT

public:

    /** Model columns */
    enum class Column {
        Type,
    	Title,
        Tags,
        Date,
        Summary,
        Resource,
        Thumbnail,
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

    /** Base standard model item class for video */
    class CORE_EXPORT Item : public QStandardItem {
    public:

        /**
         * Construct with pointer \p video
         * @param video Const pointer to video object
         * @param editable Boolean determining whether the item is editable or not
         */
        Item(const util::LearningCenterVideo* video, bool editable = false);

        /**
         * Get video
         * return Pointer to the video
         */
        const util::LearningCenterVideo* getVideo() const;

    private:
        const util::LearningCenterVideo*   _video;      /** The video data */
    };

protected:

    /** Standard model item class for displaying the video type */
    class TypeItem final : public Item {
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
	                return "Type";

	            case Qt::ToolTipRole:
	                return "Type of video";

	            default:
	                break;
            }

            return {};
        }
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

                default:
                    break;
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

                default:
                    break;
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

                default:
                    break;
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

                default:
                    break;
            }

            return {};
        }
    };

    /** Standard model item class for displaying the video resource */
    class ResourceItem final : public Item {
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
                    return "Resource";

                case Qt::ToolTipRole:
                    return "Video resource";

                default:
                    break;
            }

            return {};
        }
    };

    /** Standard model item class for displaying the video thumbnail */
    class ThumbnailItem final : public Item {
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
                    return "Thumbnail";

	            case Qt::ToolTipRole:
	                return "Video thumbnail";

	            default:
	                break;
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
         * Construct with pointer to \p video object
         * @param video Pointer to video object
         */
        Row(const util::LearningCenterVideo* video) :
            QList<QStandardItem*>()
        {
            append(new TypeItem(video));
        	append(new TitleItem(video));
            append(new TagsItem(video));
            append(new DateItem(video));
            append(new SummaryItem(video));
            append(new ResourceItem(video));
            append(new DelegateItem(video));
        }
    };

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    LearningCenterVideosModel(QObject* parent = nullptr);

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

    /**
     * Add \p video
     * @param video Pointer to video to add
     */
    void addVideo(const util::LearningCenterVideo* video);

    /** Builds a set of all video tags and emits LearningCenterVideosModel::tagsChanged(...) */
    void updateTags();

    /** Synchronize the model with the data source names */
    void synchronizeWithDsns();

private:

    /**
     * Download videos from \p dsn
     * @param dsn Videos Data Source Name (DSN)
     * @return Downloaded data
     */
    static QByteArray downloadVideosFromDsn(const QString& dsn);

public: // Action getters

    gui::StringsAction& getDsnsAction() { return _dsnsAction; }

    const gui::StringsAction& getDsnsAction() const { return _dsnsAction; }

signals:

    /**
     * Signals that the tags changed to \p tags
     * @param tags New tags
     */
    void tagsChanged(const QSet<QString>& tags);

    /** Signals that the model was populated from one or more source DSNs */
    void populatedFromDsns();

private:
    util::LearningCenterVideos      _videos;        /** Model videos */
    QSet<QString>                   _tags;          /** All tags */
    gui::StringsAction              _dsnsAction;    /** Data source names action */
    QFuture<QByteArray>             _future;        /** Future for downloading projects */
    QFutureWatcher<QByteArray>      _watcher;       /** Future watcher for downloading projects */
};

}
