// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2025 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "SortFilterProxyModel.h"

namespace mv {

/**
 * Sorting and filtering proxy model for third-party license usage
 *
 * Supports filtering by text, Core/plugin source and whether a plugin using a dependency
 * is currently instantiated.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ThirdPartyLicensesFilterModel : public SortFilterProxyModel
{
public:

    /** Plugin availability scope used by the loaded-only filter. */
    enum class PluginState {
        AllAvailable,   /** Include dependencies declared by all available plugin factories */
        LoadedOnly      /** Include plugin dependencies only when an instance is active */
    };

    /**
     * Construct the filter model with optional p parent
     * @param parent Parent object
     */
    explicit ThirdPartyLicensesFilterModel(QObject* parent = nullptr);

    /** Determine whether the source row at p row is accepted. */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

    /** Compare two source rows for sorting. */
    bool lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const override;

    /** Get the current plugin availability scope. */
    PluginState getPluginState() const;

    /** Set the plugin availability scope to p pluginState. */
    void setPluginState(PluginState pluginState);

    /** Get whether Core license rows are shown. */
    bool getShowCoreLicenses() const;

    /** Set whether Core license rows are shown. */
    void setShowCoreLicenses(bool showCoreLicenses);

    /** Get whether plugin license rows are shown. */
    bool getShowPluginLicenses() const;

    /** Set whether plugin license rows are shown. */
    void setShowPluginLicenses(bool showPluginLicenses);

private:
    PluginState _pluginState;         /** Current plugin availability scope */
    bool _showCoreLicenses;           /** Whether Core license rows are included */
    bool _showPluginLicenses;         /** Whether plugin license rows are included */
};

}
