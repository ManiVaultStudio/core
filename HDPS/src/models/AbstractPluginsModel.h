// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QStandardItemModel>

namespace mv {

/**
 * Abstract plugin manager model class
 *
 * Base standard item model for plugins
 *
 * @author Thomas Kroes
 */
class AbstractPluginsModel : public QStandardItemModel
{
public:

    /** Model columns */
    enum class Column {
        Name,       /** Item name (plugin type, factory name or plugin name) */
        Category,   /** Item category (type, factory or instance) */
        ID          /** Globally unique plugin instance identifier */
    };

    /** Standard model item class for displaying the item name */
    class NameItem final : public QStandardItem {
    public:

        /** No need for specialized constructor */
        using QStandardItem::QStandardItem;

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
    class CategoryItem final : public QStandardItem {
    public:

        /** No need for specialized constructor */
        using QStandardItem::QStandardItem;

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

    /** Standard model item class for displaying the plugin ID */
    class IdItem final : public QStandardItem {
    public:

        /** No need for specialized constructor */
        using QStandardItem::QStandardItem;

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
                    return "ID";

                case Qt::ToolTipRole:
                    return "Globally unique plugin instance identifier";
            }

            return {};
        }
    };

    /** Convenience class for combining items in a row */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct with \p name, \p category, \p id and possibly \p icon
         * @param name Item name
         * @param category Item category
         * @param id Globally unique plugin instance identifier
         * @param icon Item icon
         */
        Row(const QString& name, const QString& category, const QString& id, const QIcon& icon = QIcon());
    };

public:

    /**
     * Construct plugin manager model with \p parent
     * @param parent Pointer to parent object
     */
    AbstractPluginsModel(QObject* parent = nullptr);

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