// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "util/LearningCenterTutorial.h"

#include <QMap>
#include <QStandardItemModel>

namespace mv {

/**
 * Learning center tutorial model class
 *
 * Contains tutorial content for the learning center
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT LearningCenterTutorialsModel final : public QStandardItemModel
{
    Q_OBJECT

public:

    /** Model columns */
    enum class Column {
    	Title,
        Tags,
        Date,
        IconName,
        Summary,
        Content,
        Url,

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

    /** Base standard model item class for tutorial */
    class CORE_EXPORT Item : public QStandardItem {
    public:

        /**
         * Construct with pointer \p tutorial
         * @param tutorial Const pointer to tutorial
         * @param editable Boolean determining whether the item is editable or not
         */
        Item(const util::LearningCenterTutorial* tutorial, bool editable = false);

        /**
         * Get tutorial
         * return Pointer to the tutorial
         */
        const util::LearningCenterTutorial* getTutorial() const;

    private:
        const util::LearningCenterTutorial*   _tutorial;      /** The tutorial data */
    };

protected:

    /** Standard model item class for displaying the tutorial title */
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

    /** Standard model item class for displaying the tutorial tags */
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

    /** Standard model item class for displaying the tutorial date */
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
                    return "Date at which the tutorial was published";

                default:
                    break;
            }

            return {};
        }
    };

    /** Standard model item class for displaying the tutorial icon name */
    class IconNameItem final : public Item {
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
	                return "Icon name";

	            case Qt::ToolTipRole:
	                return "Tutorial Font Awesome icon name";

	            default:
	                break;
            }

            return {};
        }
    };

    /** Standard model item class for displaying the tutorial summary */
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
	                return "Tutorial description";

	            default:
	                break;
            }

            return {};
        }
    };

    /** Standard model item class for displaying the tutorial summary */
    class ContentItem final : public Item {
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
	                return "Content";

	            case Qt::ToolTipRole:
	                return "Tutorial content";

	            default:
	                break;
            }

            return {};
        }
    };

    /** Standard model item class for displaying the tutorial URL */
    class UrlItem final : public Item {
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
                    return "Tutorial URL";

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
         * Construct with pointer to \p tutorial object
         * @param tutorial Pointer to tutorial object
         */
        Row(const util::LearningCenterTutorial* tutorial) :
            QList<QStandardItem*>()
        {
        	append(new TitleItem(tutorial));
            append(new TagsItem(tutorial));
            append(new DateItem(tutorial));
            append(new IconNameItem(tutorial));
            append(new SummaryItem(tutorial));
            append(new ContentItem(tutorial));
            append(new UrlItem(tutorial));
        }
    };

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    LearningCenterTutorialsModel(QObject* parent = nullptr);

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
     * Add \p tutorial
     * @param tutorial Pointer to tutorial to add
     */
    void addTutorial(const util::LearningCenterTutorial* tutorial);

    /** Builds a set of all video tags and emits LearningCenterTutorialsModel::tagsChanged(...) */
    void updateTags();

signals:

    /**
     * Signals that the tags changed to \p tags
     * @param tags New tags
     */
    void tagsChanged(const QSet<QString>& tags);

private:
    util::LearningCenterTutorials   _tutorials;     /** Model tutorials */
    QSet<QString>                   _tags;          /** All tags */
};

}