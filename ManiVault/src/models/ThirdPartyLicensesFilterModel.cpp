// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ThirdPartyLicensesFilterModel.h"

#include "AbstractThirdPartyLicensesModel.h"

namespace mv {

ThirdPartyLicensesFilterModel::ThirdPartyLicensesFilterModel(QObject* parent) :
    SortFilterProxyModel(parent),
    _pluginState(PluginState::AllAvailable),
    _showCoreLicenses(true),
    _showPluginLicenses(true)
{
    setFilterColumn(static_cast<int>(AbstractThirdPartyLicensesModel::Column::Name));
}

bool ThirdPartyLicensesFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto sourceIndex = sourceModel()->index(row, 0, parent);
    if (!sourceIndex.isValid())
        return false;

    // The source model stores the complete usage record on the first-column item.
    const auto usage = sourceModel()->data(sourceIndex, Qt::UserRole + 1).value<ThirdPartyLicenseUsage>();

    if (usage.isCore && !_showCoreLicenses)
        return false;
    if (!usage.isCore && !_showPluginLicenses)
        return false;
    if (_pluginState == PluginState::LoadedOnly && !usage.isCore && usage.loadedPlugins.isEmpty())
        return false;

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(sourceIndex.siblingAtColumn(filterKeyColumn()), filterRole()).toString();
        if (!key.contains(filterRegularExpression()))
            return false;
    }

    return true;
}

bool ThirdPartyLicensesFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return lhs.data().toString().compare(rhs.data().toString(), Qt::CaseInsensitive) < 0;
}

ThirdPartyLicensesFilterModel::PluginState ThirdPartyLicensesFilterModel::getPluginState() const
{
    return _pluginState;
}

void ThirdPartyLicensesFilterModel::setPluginState(PluginState pluginState)
{
    if (_pluginState == pluginState)
        return;
    _pluginState = pluginState;
    invalidate();
}

bool ThirdPartyLicensesFilterModel::getShowCoreLicenses() const
{
    return _showCoreLicenses;
}

void ThirdPartyLicensesFilterModel::setShowCoreLicenses(bool showCoreLicenses)
{
    if (_showCoreLicenses == showCoreLicenses)
        return;
    _showCoreLicenses = showCoreLicenses;
    invalidate();
}

bool ThirdPartyLicensesFilterModel::getShowPluginLicenses() const
{
    return _showPluginLicenses;
}

void ThirdPartyLicensesFilterModel::setShowPluginLicenses(bool showPluginLicenses)
{
    if (_showPluginLicenses == showPluginLicenses)
        return;
    _showPluginLicenses = showPluginLicenses;
    invalidate();
}

}
