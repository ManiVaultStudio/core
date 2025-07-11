// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "models/StandardItemModel.h"
#include "models/ProjectsModelProject.h"

#include <QMap>

namespace mv {

/**
 * Abstract projects model class
 *
 * Base standard item model for projects.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractProjectsModel : public StandardItemModel
{
    Q_OBJECT

public:

    /** Model columns */
    enum class Column {
        Title,                  /** Title of the project */
        Downloaded,             /** Whether the project has been downloaded before */
        Group,                  /** Group title of the project */
        IsGroup,                /** Whether the item is a project group */
        Tags,                   /** Tags of the project */
        Date,                   /** Date of the project */
        IconName,               /** Icon name of the project */
        Summary,                /** Summary of the project */
        Url,                    /** URL of the project */
        MinimumCoreVersion,     /** Minimum ManiVault application core version */
        RequiredPlugins,        /** Required plugins for the project */
        MissingPlugins,         /** Missing plugins for the project */
        DownloadSize,           /** Download size of the ManiVault project */
        SystemCompatibility,    /** System compatibility of the project */
        IsStartup,              /** Whether the project can be opened at application startup */

        Count                   /** Number of columns in the model */
    };

    /** Base standard model item class for project */
    class CORE_EXPORT Item : public QStandardItem, public QObject {
    public:

        /**
         * Construct with pointer \p project
         * @param project Const pointer to project
         * @param editable Boolean determining whether the item is editable or not
         */
        Item(const util::ProjectsModelProject* project, bool editable = false);

        /**
         * Get project
         * return Pointer to the project
         */
        const util::ProjectsModelProject* getProject() const;

    private:
        const util::ProjectsModelProject*   _project;      /** The project data */
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

    /** Standard model item class for displaying whether the project has been downloaded before */
    class DownloadedItem final : public Item {
    public:

        /**
         * Construct with pointer \p project
         * @param project Const pointer to project
         * @param editable Boolean determining whether the item is editable or not
         */
        DownloadedItem(const util::ProjectsModelProject* project, bool editable = false);

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
	                return "Downloaded";

	            case Qt::ToolTipRole:
	                return "Whether the project has been downloaded before";

	            default:
	                break;
            }

            return {};
        }
    };

    /** Standard model item class for displaying the project group title */
    class GroupItem final : public Item {
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
                return "Group";

            case Qt::ToolTipRole:
                return "Group title";

            default:
                break;
            }

            return {};
        }
    };

    /** Standard model item class for displaying whether the item is a project group */
    class IsGroupItem final : public Item {
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
                return "Is group";

            case Qt::ToolTipRole:
                return "Whether the item is a project group";

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

    /** Standard model item class for displaying the project minimum application core version */
    class MinimumCoreVersionItem final : public Item {
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
	                return "Min. app core version";

	            case Qt::ToolTipRole:
	                return "Minimum ManiVault Studio application core version";

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

    /** Standard model item class for displaying the download size */
    class DownloadSizeItem final : public Item {
    public:

        /**
         * Construct with pointer \p project
         * @param project Const pointer to project
         * @param editable Boolean determining whether the item is editable or not
         */
        DownloadSizeItem(const util::ProjectsModelProject* project, bool editable = false);

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
	                return "Download Size";

	            case Qt::ToolTipRole:
	                return "Download Size of the project";

	            default:
	                break;
            }

            return {};
        }
    };

    /** Standard model item class for displaying the system compatibility with the project */
    class SystemCompatibilityItem final : public Item {
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
                return "System compatibility";

            case Qt::ToolTipRole:
                return "System compatibility with the project";

            default:
                break;
            }

            return {};
        }
    };

    /** Standard model item class for displaying whether the project is a startup project */
    class IsStartupItem final : public Item {
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
	                return "IsStartup";

	            case Qt::ToolTipRole:
	                return "Whether the project can be opened at application startup";

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
        Row(const util::ProjectsModelProject* project) :
            QList<QStandardItem*>()
        {
        	append(new TitleItem(project));
        	append(new DownloadedItem(project));
        	append(new GroupItem(project));
        	append(new IsGroupItem(project));
            append(new TagsItem(project));
            append(new DateItem(project));
            append(new IconNameItem(project));
            append(new SummaryItem(project));
            append(new UrlItem(project));
            append(new MinimumCoreVersionItem(project));
            append(new RequiredPluginsItem(project));
            append(new MissingPluginsItem(project));
            append(new DownloadSizeItem(project));
            append(new SystemCompatibilityItem(project));
            append(new IsStartupItem(project));
        }
    };

public:

    /**
     * Construct with pointer to \p parent object
     * @param populationMode Population mode of the model (automatic/manual)
     * @param parent Pointer to parent object
     */
    AbstractProjectsModel(const PopulationMode& populationMode = PopulationMode::Automatic, QObject* parent = nullptr);

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
     * Add project group with \p groupTitle
     * @param groupTitle Title of the group
     */
    void addProjectGroup(const QString& groupTitle = "");

    /**
     * Add \p project
     * @param project Pointer to project to add
     * @param groupTitle Title of the group to which the project should be added
     */
    void addProject(const util::ProjectsModelProject* project, const QString& groupTitle = "");

    /** Builds a set of all project tags and emits ProjectDatabaseModel::tagsChanged(...) */
    void updateTags();

    /**
     * Get the project at \p index
     * @return Project at index
     */
    const util::ProjectsModelProject* getProject(const QModelIndex& index) const;

    /**
     * Get the projects
     * @return Projects
     */
    const util::ProjectDatabaseProjects& getProjects() const;

signals:

    /**
     * Signals that the tags changed to \p tags
     * @param tags New tags
     */
    void tagsChanged(const QSet<QString>& tags);

private:
    util::ProjectDatabaseProjects   _projects;          /** Model projects */
    QSet<QString>                   _tags;              /** All tags */
};

}
