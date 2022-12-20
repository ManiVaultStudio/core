#pragma once

#include <QStandardItemModel>

/**
 * Plugin manager model class
 *
 * Model implementation for the plugin manager
 *
 * @author Thomas Kroes
 */
class PluginManagerModel : public QStandardItemModel
{
public:

    /** Model columns */
    enum class Column {
        Name,       /** Item name (plugin type, factory name or plugin name) */
        Category    /** Item category (type, factory or instance) */
    };

    /** Column name and tooltip */
    static QMap<Column, QPair<QString, QString>> columnInfo;

public:

    /**
     * Construct plugin manager model with \p parent
     * @param parent Pointer to parent object
     */
    PluginManagerModel(QObject* parent = nullptr);

    void removeItem(const QModelIndex& index);

private:

    /** Initializes the model from the contents of the plugin manager */
    void synchronizeWithPluginManager();
};
