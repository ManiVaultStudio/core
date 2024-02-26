// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QStandardItemModel>

namespace mv {

namespace plugin {
    class PluginFactory;
}

/**
 * Plugin factories model class
 *
 * Base standard item model for plugin factories
 *
 * @author Thomas Kroes
 */
class AbstractPluginFactoriesModel : public QStandardItemModel
{
public:

    /** Model columns */
    enum class Column {
        Type,               /** Type of plugin */
        Kind,               /** Kind of plugin */
        Version,            /** Plugin version */
        NumberOfInstances,  /** Number of current instances */

        Count
    };

    /** Base standard model item class for plugin factory item */
    class Item : public QStandardItem {
    public:

        /**
         * Construct with pointer to \p pluginFactory
         * @param pluginFactory Pointer to plugin factory
         */
        Item(plugin::PluginFactory* pluginFactory);

        /**
         * Get plugin factory
         * return Pointer to plugin factory
         */
        plugin::PluginFactory* getPluginFactory() const;

    private:
        plugin::PluginFactory*  _pluginFactory;     /** Pointer to plugin factory */
    };

    /** Item class for displaying the plugin factory type */
    class TypeItem final : public Item {
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
                    return "Type";

                case Qt::ToolTipRole:
                    return "Plugin type";
            }

            return {};
        }
    };

    /** Item class for displaying the plugin factory kind */
    class KindItem final : public Item {
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
                    return "Kind";

                case Qt::ToolTipRole:
                    return "Plugin kind";
            }

            return {};
        }
    };

    /** Item class for displaying the plugin factory version */
    class VersionItem final : public Item {
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
                    return "Version";

                case Qt::ToolTipRole:
                    return "Plugin version";
            }

            return {};
        }
    };

    /** Item class for displaying the number of current plugin instances */
    class NumberOfInstancesItem final : public Item {
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
                    return "Number of instances";

                case Qt::ToolTipRole:
                    return "Number of plugin instances";
            }

            return {};
        }
    };

    /** Convenience class for combining items in a row */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct with pointer to \p pluginFactory
         * @param pluginFactory Pointer to plugin factory
         */
        Row(plugin::PluginFactory* pluginFactory);
    };

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractPluginFactoriesModel(QObject* parent = nullptr);

    /**
     * Get header data for \p section, \p orientation and display \p role
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

}