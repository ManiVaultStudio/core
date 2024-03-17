// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QMap>
#include <QPair>
#include <QStandardItemModel>
#include <QString>

/**
 * Workspace inventory model class
 *
 * Model class which contains an inventory of all available workspaces.
 *
 * @author Thomas Kroes
 */
class WorkspaceInventoryModel final : public QStandardItemModel
{
public:

    /** Model columns */
    enum class Column {
        SummaryDelegate,    /** Shows a delegate item with title, description and tags */
        Icon,               /** Workspace icon */
        Name,               /** Workspace name */
        Description,        /** Workspace description */
        Tags,               /** Workspace tags */
        FilePath,           /** Location of the workspace on disk */
        Type                /** Type of workspace (built-in or recent) */
    };

    /** Column name and tooltip */
    static QMap<Column, QPair<QString, QString>> columns;

public:

    /**
     * Construct workspace inventory model with \p parent
     * @param parent Pointer to parent object
     */
    WorkspaceInventoryModel(QObject* parent = nullptr);

private:

    /** Initializes the model from the contents of the workspace manager */
    void synchronizeWithWorkspaceManager();
};
