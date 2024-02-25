// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QStandardItemModel>

namespace mv {

//namespace plugin {
//    class PluginFactory;
//}
//
///**
// * Abstract plugin manager model class
// *
// * Base standard item model for plugins
// *
// * @author Thomas Kroes
// */
//class AbstractPluginsModel : public QStandardItemModel
//{
//public:
//
//    /** Model columns */
//    enum class Column {
//        Name,       /** Item name (plugin type, factory name or plugin name) */
//        Category,   /** Item category (type, factory or instance) */
//        ID          /** Globally unique plugin instance identifier */
//    };
//
//    /** Base standard model item class for task */
//    class Item : public QStandardItem {
//    public:
//
//        /**
//         * Construct with pointer to \p pluginFactory
//         * @param pluginFactory Pointer to plugin factory
//         */
//        Item(plugin::PluginFactory* pluginFactory);
//
//        /**
//         * Get plugin factory
//         * return Pointer to plugin factory
//         */
//        plugin::PluginFactory* getPluginFactory() const;
//
//    private:
//        plugin::PluginFactory*  _pluginFactory;     /** Pointer to plugin factory */
//    };
//
//    /** Standard model item class for displaying the item name */
//    class NameItem final : public Item {
//    public:
//
//        /** No need for specialized constructor */
//        using Item::Item;
//
//        /**
//         * Get model data for \p role
//         * @return Data for \p role in variant form
//         */
//        QVariant data(int role = Qt::UserRole + 1) const override;
//
//        /**
//         * Get header data for \p orientation and \p role
//         * @param orientation Horizontal/vertical
//         * @param role Data role
//         * @return Header data
//         */
//        static QVariant headerData(Qt::Orientation orientation, int role) {
//            switch (role) {
//                case Qt::DisplayRole:
//                case Qt::EditRole:
//                    return "Name";
//
//                case Qt::ToolTipRole:
//                    return "Name of the item";
//            }
//
//            return {};
//        }
//    };
//
//    /** Standard model item class for displaying the item category */
//    class CategoryItem final : public Item {
//    public:
//
//        /** No need for specialized constructor */
//        using Item::Item;
//
//        /**
//         * Get model data for \p role
//         * @return Data for \p role in variant form
//         */
//        QVariant data(int role = Qt::UserRole + 1) const override;
//
//        /**
//         * Get header data for \p orientation and \p role
//         * @param orientation Horizontal/vertical
//         * @param role Data role
//         * @return Header data
//         */
//        static QVariant headerData(Qt::Orientation orientation, int role) {
//            switch (role) {
//                case Qt::DisplayRole:
//                case Qt::EditRole:
//                    return "Category";
//
//                case Qt::ToolTipRole:
//                    return "Item category";
//            }
//
//            return {};
//        }
//    };
//
//    /** Standard model item class for displaying the plugin ID */
//    class IdItem final : public Item {
//    public:
//
//        /** No need for specialized constructor */
//        using Item::Item;
//
//        /**
//         * Get model data for \p role
//         * @return Data for \p role in variant form
//         */
//        QVariant data(int role = Qt::UserRole + 1) const override;
//
//        /**
//         * Get header data for \p orientation and \p role
//         * @param orientation Horizontal/vertical
//         * @param role Data role
//         * @return Header data
//         */
//        static QVariant headerData(Qt::Orientation orientation, int role) {
//            switch (role) {
//                case Qt::DisplayRole:
//                case Qt::EditRole:
//                    return "ID";
//
//                case Qt::ToolTipRole:
//                    return "Globally unique identifier of the plugin";
//            }
//
//            return {};
//        }
//    };
//
//    /** Convenience class for combining items in a row */
//    class Row final : public QList<QStandardItem*>
//    {
//    public:
//
//        /**
//         * Construct with pointer to \p pluginFactory
//         * @param pluginFactory Pointer to plugin factory
//         */
//        Row(plugin::PluginFactory* pluginFactory);
//    };
//
//public:
//
//    /**
//     * Construct plugin manager model with \p parent
//     * @param parent Pointer to parent object
//     */
//    AbstractPluginsModel(QObject* parent = nullptr);
//};

}