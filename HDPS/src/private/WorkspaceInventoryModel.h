#pragma once

#include <QStandardItemModel>

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
        Icon,           /** Workspace icon */
        Name,           /** Workspace name */
        Description,    /** Workspace description */
        Tags,           /** Workspace tags */
        FilePath,       /** Location of the workspace on disk */
        Summary         /** Shows a delegate item with title, description and tags */
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
