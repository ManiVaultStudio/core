#pragma once

#include <QStandardItemModel>

/**
 * Workspaces model class
 *
 * Model class for st for the plugin manager.
 *
 * @author Thomas Kroes
 */
class WorkspacesModel final : public QStandardItemModel
{
public:

    /** Model columns */
    enum class Column {
        Name,       /** Item name (plugin type, factory name or plugin name) */
        Category,   /** Item category (type, factory or instance) */
        ID          /** Globally unique plugin instance identifier */
    };

    /** Column name and tooltip */
    static QMap<Column, QPair<QString, QString>> columns;

public:

    /**
     * Construct plugin manager model with \p parent
     * @param parent Pointer to parent object
     */
    PluginManagerModel(QObject* parent = nullptr);

private:

    /** Initializes the model from the contents of the plugin manager */
    void synchronizeWithPluginManager();
};
