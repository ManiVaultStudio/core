// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "models/StandardItemModel.h"

#include <QMap>

namespace mv {

/**
 * Abstract hardware spec model class
 *
 * Base standard item model for hardware specification.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractHardwareSpecModel : public StandardItemModel
{
    Q_OBJECT

public:

    ///** Model columns */
    //enum class Column {
    //	Title,
    //    Group,
    //    IsGroup,
    //    Tags,
    //    Date,
    //    IconName,
    //    Summary,
    //    Url,
    //    MinimumCoreVersion,
    //    RequiredPlugins,
    //    MissingPlugins,

    //    Count
    //};

    ///** Header strings for several data roles */
    //struct ColumHeaderInfo {
    //    QString     _display;   /** Header string for display role */
    //    QString     _edit;      /** Header string for edit role */
    //    QString     _tooltip;   /** Header string for tooltip role */
    //};

    ///** Column name and tooltip */
    //static QMap<Column, ColumHeaderInfo> columnInfo;

    /** Base standard model item class for project */
    //class CORE_EXPORT Item : public QStandardItem {
    //public:

    //    /**
    //     * Construct with pointer \p project
    //     * @param project Const pointer to project
    //     * @param editable Boolean determining whether the item is editable or not
    //     */
    //    Item(const util::ProjectsModelProject* project, bool editable = false);

    //    /**
    //     * Get project
    //     * return Pointer to the project
    //     */
    //    const util::ProjectsModelProject* getProject() const;

    //private:
    //    const util::ProjectsModelProject*   _project;      /** The project data */
    //};

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractHardwareSpecModel(QObject* parent = nullptr);

    /**
     * Get header data for \p section, \p orientation and display \p role
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    //util::ProjectDatabaseProjects   _projects;  /** Model projects */
    //QSet<QString>                   _tags;      /** All tags */
};

}
