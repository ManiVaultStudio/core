// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "models/StandardItemModel.h"

#include "util/ProjectCenterProject.h"
#include "util/FileDownloader.h"

#include <QMap>

namespace mv {

/**
 * Project center model class
 *
 * Contains project content for the project center.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ProjectCenterModel final : public StandardItemModel
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
        Url,
        MinimumVersionMajor,
        MinimumVersionMinor,
        RequiredPlugins,
        MissingPlugins,

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

    /** Base standard model item class for project */
    class CORE_EXPORT Item : public QStandardItem {
    public:

        /**
         * Construct with pointer \p project
         * @param project Const pointer to project
         * @param editable Boolean determining whether the item is editable or not
         */
        Item(const util::ProjectCenterProject* project, bool editable = false);

        /**
         * Get project
         * return Pointer to the project
         */
        const util::ProjectCenterProject* getProject() const;

    private:
        const util::ProjectCenterProject*   _project;      /** The project data */
    };

protected:

    /** Standard model item class for displaying the project title */
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

    /** Standard model item class for displaying the project tags */
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

    /** Standard model item class for displaying the project date */
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
                    return "Date at which the project was published";

                default:
                    break;
            }

            return {};
        }
    };

    /** Standard model item class for displaying the project icon name */
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
	                return "Project Font Awesome icon name";

	            default:
	                break;
            }

            return {};
        }
    };

    /** Standard model item class for displaying the project summary */
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
	                return "Project description";

	            default:
	                break;
            }

            return {};
        }
    };

    /** Standard model item class for displaying the project URL */
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
                    return "Project URL";

                default:
                    break;
            }

            return {};
        }
    };

    /** Standard model item class for displaying the project minimum application version (major) */
    class MinimumVersionMajorItem final : public Item {
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
	                return "Min. app version (major)";

	            case Qt::ToolTipRole:
	                return "Minimum ManiVault Studio application version (major)";

	            default:
	                break;
            }

            return {};
        }
    };

    /** Standard model item class for displaying the project minimum application version (minor) */
    class MinimumVersionMinorItem final : public Item {
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
	                return "Min. app version (minor)";

	            case Qt::ToolTipRole:
	                return "Minimum ManiVault Studio application version (minor)";

	            default:
	                break;
            }

            return {};
        }
    };

    /** Standard model item class for displaying required plugins */
    class RequiredPluginsItem final : public Item {
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
	                return "Required plugins";

	            case Qt::ToolTipRole:
	                return "Required plugins for the project";

	            default:
	                break;
            }

            return {};
        }
    };

    /** Standard model item class for displaying missing plugins */
    class MissingPluginsItem final : public Item {
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
                return "Missing plugins";

            case Qt::ToolTipRole:
                return "Missing plugins for the project";

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
         * Construct with pointer to \p project object
         * @param project Pointer to project object
         */
        Row(const util::ProjectCenterProject* project) :
            QList<QStandardItem*>()
        {
        	append(new TitleItem(project));
            append(new TagsItem(project));
            append(new DateItem(project));
            append(new IconNameItem(project));
            append(new SummaryItem(project));
            append(new UrlItem(project));
            append(new MinimumVersionMajorItem(project));
            append(new MinimumVersionMinorItem(project));
        }
    };

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    ProjectCenterModel(QObject* parent = nullptr);

    /**
     * Get header data for \p section, \p orientation and display \p role
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * Set the source URL to \p sourceUrl
     * @param sourceUrl Source URL
     */
    void setSourceUrl(const QUrl& sourceUrl);

    /**
     * Get tags
     * @return All tags
     */
    QSet<QString> getTagsSet() const;

    /**
     * Add \p project
     * @param project Pointer to project to add
     */
    void addProject(const util::ProjectCenterProject* project);

    /** Builds a set of all video tags and emits ProjectCenterModel::tagsChanged(...) */
    void updateTags();

    /**
     * Get the project at \p index
     * @return Project at index
     */
    const util::ProjectCenterProject* getProject(const QModelIndex& index) const;

    /**
     * Get the projects
     * @return Projects
     */
    const util::ProjectCenterProjects& getProjects() const;

signals:

    /**
     * Signals that the tags changed to \p tags
     * @param tags New tags
     */
    void tagsChanged(const QSet<QString>& tags);

    /** Signals that the model was populated from the website */
    void populatedFromSourceUrl();

private:
    util::ProjectCenterProjects     _projects;          /** Model projects */
    QSet<QString>                   _tags;              /** All tags */
    util::FileDownloader            _fileDownloader;    /** For downloading the project center JSON file */
};

}
