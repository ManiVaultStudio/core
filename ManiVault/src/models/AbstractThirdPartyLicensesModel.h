// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2025 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "StandardItemModel.h"
#include "PluginMetadata.h"

#include <QList>
#include <QStandardItem>

namespace mv {

/** Aggregated usage information for one third-party dependency license. */
struct CORE_EXPORT ThirdPartyLicenseUsage
{
    plugin::ThirdPartyLicense license;    /** Dependency license details */
    bool isCore = false;                  /** Whether the dependency is used by ManiVault Core */
    QStringList availablePlugins;         /** Plugins that declare the dependency */
    QStringList loadedPlugins;            /** Instantiated plugins that use the dependency */
};

/** Collection of aggregated third-party dependency license usage records. */
using ThirdPartyLicenseUsages = std::vector<ThirdPartyLicenseUsage>;

/**
 * Abstract standard item model for aggregated third-party license usage
 *
 * Provides the common columns, item types and storage used by third-party license models.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractThirdPartyLicensesModel : public StandardItemModel
{
public:

    /** Model columns */
    enum class Column {
        Name,       /** Dependency name */
        License,    /** Dependency license */
        Core,       /** Whether the dependency is used by Core */
        Plugins,    /** Plugins that use the dependency */

        Count       /** Number of columns */
    };

    /** Base item class that stores the complete usage record for a row. */
    class CORE_EXPORT Item : public QStandardItem
    {
    public:

        /**
         * Construct with p usage and optional display p text
         * @param usage Aggregated third-party license usage
         * @param text Display text for the item
         */
        Item(const ThirdPartyLicenseUsage& usage, const QString& text = QString());

        /**
         * Get item data for p role
         * @param role Data role
         * @return Item data
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /** Get the complete usage record stored by the item */
        const ThirdPartyLicenseUsage& getUsage() const;

    private:
        ThirdPartyLicenseUsage _usage;
    };

    /** Item class for displaying the dependency name. */
    class CORE_EXPORT NameItem final : public Item
    {
    public:
        using Item::Item;

        /** Get header data for the dependency name column. */
        static QVariant headerData(Qt::Orientation orientation, int role);
    };

    /** Item class for displaying the dependency license. */
    class CORE_EXPORT LicenseItem final : public Item
    {
    public:
        using Item::Item;

        /** Get header data for the dependency license column. */
        static QVariant headerData(Qt::Orientation orientation, int role);
    };

    /** Item class for displaying whether a dependency is used by Core. */
    class CORE_EXPORT CoreItem final : public Item
    {
    public:
        using Item::Item;

        /** Get header data for the Core usage column. */
        static QVariant headerData(Qt::Orientation orientation, int role);
    };

    /** Item class for displaying the plugins that use a dependency. */
    class CORE_EXPORT PluginsItem final : public Item
    {
    public:
        using Item::Item;

        /** Get header data for the plugin usage column. */
        static QVariant headerData(Qt::Orientation orientation, int role);
    };

protected:

    /** Convenience class for combining the four license usage columns into a row. */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /** Construct a row from p usage. */
        explicit Row(const ThirdPartyLicenseUsage& usage);
    };

public:

    /**
     * Construct the model with optional p parent
     * @param parent Parent object
     */
    explicit AbstractThirdPartyLicensesModel(QObject* parent = nullptr);

    /** Replace the model contents with p usages. */
    void setLicenseUsages(const ThirdPartyLicenseUsages& usages);

    /** Get all usage records currently held by the model. */
    ThirdPartyLicenseUsages getLicenseUsages() const;

    /** Get the usage record represented by p index. */
    ThirdPartyLicenseUsage getLicenseUsage(const QModelIndex& index) const;

protected:

    /** Get header data for p section, p orientation and p role. */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

}

Q_DECLARE_METATYPE(mv::ThirdPartyLicenseUsage)
