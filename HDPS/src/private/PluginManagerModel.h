// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QStandardItemModel>

/**
 * Plugin manager model class
 *
 * Model implementation for the plugin manager.
 *
 * @author Thomas Kroes
 */
class PluginManagerModel : public QStandardItemModel
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
