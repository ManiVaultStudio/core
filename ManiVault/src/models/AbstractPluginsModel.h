// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "StandardItemModel.h"

#include <QList>
#include <QStandardItem>

#include "Plugin.h"

namespace mv {

/**
 * Abstract plugin manager model class
 *
 * Base standard item model for plugins
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractPluginsModel : public StandardItemModel
{
public:

    /** Population */
    enum class PopulationMode {
        Manual,         /** ...done manually with \p setPlugins() */
        Automatic       /** ...performed every time a plugin is added or removed */
    };

    /** Model columns */
    enum class Column {
        Name,       /** Item name (plugin type, factory name or plugin name) */
        Category,   /** Item category (type, factory or instance) */
        Id,         /** Globally unique plugin instance identifier */

        Count
    };

    /** Base standard model item class for a dataset */
    class CORE_EXPORT Item : public QStandardItem {
    public:

        /**
         * Construct with pointer to the \p plugin and \p title
         * @param plugin Pointer to plugin (maybe nullptr)
         * @param title Item title
         */
        Item(plugin::Plugin* plugin, const QString& title);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /**
         * Get plugin
         * return Pointer to plugin (maybe nullptr)
         */
        plugin::Plugin* getPlugin() const;

    private:
        plugin::Plugin*   _plugin;    /** Pointer to plugin (maybe nullptr) */
    };

    /** Standard model item class for displaying the item name */
    class CORE_EXPORT NameItem final : public Item {
    public:

        /** No need for specialized constructor */
        using Item::Item;

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
                    return "Name";

                case Qt::ToolTipRole:
                    return "Item name (plugin type, factory name or plugin name)";
            }

            return {};
        }
    };

    /** Standard model item class for displaying the item category */
    class CORE_EXPORT CategoryItem final : public Item {
    public:

        /** No need for specialized constructor */
        using Item::Item;

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
                    return "Category";

                case Qt::ToolTipRole:
                    return "Item category (type, factory or instance)";
            }

            return {};
        }
    };

    /** Standard model item class for displaying the plugin Id */
    class CORE_EXPORT IdItem final : public Item, public QObject {
    public:

        /** No need for specialized constructor */
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
                    return "Id";

                case Qt::ToolTipRole:
                    return "Globally unique plugin instance identifier";
            }

            return {};
        }
    };

protected:

    /** Convenience class for combining items in a row */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct with pointer to \p plugin, \p name, \p category, \p id and possibly \p icon
         * @param plugin Pointer to plugin (maybe nullptr)
         * @param name Item name
         * @param category Item category
         * @param id Globally unique plugin instance identifier
         * @param icon Item icon
         */
        Row(plugin::Plugin* plugin, const QString& name, const QString& category, const QString& id, const QIcon& icon = QIcon()) : QList<QStandardItem*>()
        {
            auto nameItem = new NameItem(plugin, name);

            if (!icon.isNull())
                nameItem->setIcon(icon);

            append(nameItem);
            append(new CategoryItem(plugin, category));
            append(new IdItem(plugin, id));
        }
    };

public:

    /**
     * Construct plugin manager model with \p parent
     * @param parent Pointer to parent object
     */
    AbstractPluginsModel(PopulationMode populationMode = PopulationMode::Automatic, QObject* parent = nullptr);

    /**
     * Get current population mode
     * @return Population mode
     */
    PopulationMode getPopulationMode() const;

    /**
     * Set population mode to \p populationMode
     * @param populationMode Population mode
     */
    void setPopulationMode(PopulationMode populationMode);

    /**
     * Get plugins
     * @return Vector of plugins
     */
    virtual plugin::Plugins getPlugins() const = 0;

    /**
     * Get plugin for \p modelIndex
     * @param modelIndex Index to retrieve
     * @return Pointer to plugin (maybe nullptr)
     */
    virtual plugin::Plugin* getPlugin(const QModelIndex& modelIndex) const = 0;

    /**
     * Set the \p plugins from which can be picked (mode is set to Mode::Manual)
     * @param plugins Vector of plugins
     */
    virtual void setPlugins(const plugin::Plugins& plugins) = 0;

protected:

    /** Synchronize the plugins with the core */
    virtual void populateFromPluginManager() = 0;

    /**
     * Add \p plugin to the model
     * @param plugin Pointer to plugin
     */
    virtual void addPlugin(plugin::Plugin* plugin) = 0;

    /**
     * Remove \p plugin from the model
     * @param plugin Pointer to plugin
     */
    virtual void removePlugin(plugin::Plugin* plugin) = 0;

    /**
     * Get header data for \p section, \p orientation and display \p role
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    PopulationMode      _populationMode;    /** Population mode (e.g. manual or automatic) */
};

}
