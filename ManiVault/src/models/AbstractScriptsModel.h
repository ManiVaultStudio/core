// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "StandardItemModel.h"

#include <QList>
#include <QStandardItem>

namespace mv {

/**
 * Scripts model class
 *
 * Base standard item model for scripts
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractScriptsModel : public StandardItemModel
{
public:

    /** Model columns */
    enum class Column {
        Name,               /** Item name */
        Version,            /** Plugin version */
        NumberOfInstances,  /** Number of current instances */

        Count
    };

    /** Base standard model item class for plugin factory item */
    class CORE_EXPORT Item : public QStandardItem {
    public:

        /**
         * Construct with \p type and pointer to \p pluginFactory
         * @param type View type
         * @param pluginFactory Pointer to plugin factory
         */
        Item(const QString& type, plugin::PluginFactory* pluginFactory);

        /**
         * Get type
         * return Plugin type string
         */
        QString getType() const;

        /**
         * Get plugin factory
         * return Pointer to plugin factory
         */
        plugin::PluginFactory* getPluginFactory() const;

    private:
        const QString           _type;              /** Plugin type */
        plugin::PluginFactory*  _pluginFactory;     /** Pointer to plugin factory */
    };

    /** Item class for displaying the plugin factory kind */
    class CORE_EXPORT NameItem final : public Item {
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
                    return "Name";

                case Qt::ToolTipRole:
                    return "Item name";
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
         * Construct with \p type and pointer to \p pluginFactory
         * @param type View type
         * @param pluginFactory Pointer to plugin factory
         */
        Row(const QString& type, plugin::PluginFactory* pluginFactory) : QList<QStandardItem*>()
        {
            append(new NameItem(type, pluginFactory));
        }

        /**
         * Construct with \p type
         * @param type View type
         */
        Row(const QString& type) : Row(type, nullptr) {}

        /**
         * Construct with pointer to \p pluginFactory
         * @param pluginFactory Pointer to plugin factory
         */
        Row(plugin::PluginFactory* pluginFactory) : Row("", pluginFactory) {}
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