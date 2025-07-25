// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "models/StandardItemModel.h"
#include "models/ProjectsModelProject.h"

#include "actions/StringsAction.h"
#include "actions/TriggerAction.h"

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
        LastModified,           /** Last modified date of the project */
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
        Sha,                    /** SHA-256 cryptographic hash of the project */
        ProjectsJsonDsn,        /** Data Source Name (DSN) of the projects JSON file, used for loading the project */

        Count                   /** Number of columns in the model */
    };

    /** Base standard model item class for project */
    class CORE_EXPORT Item : public QStandardItem, public QObject {
    public:

        /**
         * Construct with shared pointer \p project
         * @param project Shared pointer to project
         * @param editable Boolean determining whether the item is editable or not
         */
        Item(util::ProjectsModelProjectSharedPtr project, bool editable = false);

        /**
         * Get project
         * return Shared pointer to the project
         */
        util::ProjectsModelProjectSharedPtr getProject() const;

    private:
        util::ProjectsModelProjectSharedPtr   _project;      /** Shared pointer to the project */
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

    /** Standard model item class for displaying the download size */
    class LastModifiedItem final : public Item {
    public:

        /**
         * Construct with pointer \p project
         * @param project Const pointer to project
         * @param editable Boolean determining whether the item is editable or not
         */
        LastModifiedItem(util::ProjectsModelProjectSharedPtr project, bool editable = false);

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
	                return "Last Modified";

	            case Qt::ToolTipRole:
	                return "Date and time when the project was last modified";

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
        DownloadedItem(util::ProjectsModelProjectSharedPtr project, bool editable = false);

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
        DownloadSizeItem(util::ProjectsModelProjectSharedPtr project, bool editable = false);

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

    /** Standard model item class for displaying the project SHA */
    class ShaItem final : public Item {
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
	                return "SHA";

	            case Qt::ToolTipRole:
	                return "SHA-256 hash of the project";

	            default:
	                break;
            }

            return {};
        }
    };

    /** Standard model item class for displaying to which projects JSON DSN a row belongs */
    class ProjectsJsonDsnItem final : public Item {
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
	                return "Projects DSN";

	            case Qt::ToolTipRole:
	                return "Projects Data Source Name";

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
        Row(util::ProjectsModelProjectSharedPtr project) :
            QList<QStandardItem*>()
        {
        	append(new TitleItem(project));
        	append(new LastModifiedItem(project));
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
            append(new ShaItem(project));
            append(new ProjectsJsonDsnItem(project));
        }
    };

public:

    /**
     * Construct with population \p mode and pointer to \p parent object
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

private: // Add/remove DSNs

    /**
     * Add \dsn to the model
     * @param dsn Data Source Name (DSN) to add
     */
    void addDsn(const QUrl& dsn);

    /**
     * Remove \dsn from the model
     * @param dsn Data Source Name (DSN) to remove
     */
    void removeDsn(const QUrl& dsn);

public: // Population

    /**
     * Populate the model from the Data Source Names (DSN)
     * @param jsonByteArray Byte array containing the JSON data to populate from
     * @param dsnIndex Index of the DSN to populate from
     * @param jsonLocation Location of the JSON file
     */
    void populateFromJsonByteArray(const QByteArray& jsonByteArray, std::int32_t dsnIndex, const QString& jsonLocation);

    /**
     * Populate the model from a JSON file at \p filePath
     * @param filePath Path to the JSON file containing the projects data
     */
    void populateFromJsonFile(const QString& filePath);

public: // Tags

    /** Builds a set of all project tags and emits ProjectDatabaseModel::tagsChanged(...) */
    void updateTags();

    /**
     * Get tags
     * @return All tags
     */
    QSet<QString> getTagsSet() const;

public: // Project getters

	/**
     * Get the project at \p index
     * @return Shared pointer to project at index
     */
    util::ProjectsModelProjectSharedPtr getProject(const QModelIndex& index) const;

protected:

    /** Begin the population of the model */
    void beginPopulate();

    /**
     * Populate the model with the given projects
     * @param projects Shared pointers to projects to populate the model with
     */
    virtual void populate(util::ProjectsModelProjectSharedPtrs projects);

    /** End the population of the model */
    void endPopulate();

public: // Add/remove projects

    /**
     * Add \p project
     * @param project Shared pointer to project to add
     * @param parentIndex Parent index to add the project to (defaults to root)
     */
    void addProject(util::ProjectsModelProjectSharedPtr project, const QModelIndex& parentIndex = QModelIndex());

    /**
     * Remove project with \p index from the model
     * @param index Index of the project to remove
     */
    void removeProject(const QModelIndex& index);

private:

    /** Get rid of orphaned projects (project for which the projects DSN is not in the list anymore) */
    void purge();

public: // Action getters

    gui::StringsAction& getDsnsAction() { return _dsnsAction; }
    gui::TriggerAction& getEditDsnsAction() { return _editDsnsAction; }

    const gui::StringsAction& getDsnsAction() const { return _dsnsAction; }
    const gui::TriggerAction& getEditDsnsAction() const { return _editDsnsAction; }

signals:

    /**
     * Signals that the tags changed to \p tags
     * @param tags New tags
     */
    void tagsChanged(const QSet<QString>& tags);

    /** Signals that the model is about to be populated */
    void aboutToBePopulated();

    /** Signals that the model has been re-populated */
    void populated();

private:
    QSet<QString>           _tags;                  /** All tags */
    gui::StringsAction      _dsnsAction;            /** Data source names action */
    gui::TriggerAction      _editDsnsAction;        /** Edit data source names action */
    std::int32_t            _numberOfPopulators;    /** Number of populators currently populating the model */
};

}
