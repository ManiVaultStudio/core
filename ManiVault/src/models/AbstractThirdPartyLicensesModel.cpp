// SPDX-License-Identifier: LGPL-3.0-or-later

#include "AbstractThirdPartyLicensesModel.h"

namespace mv {

/** User role used to expose a complete usage record to proxy models. */
namespace {
constexpr int LicenseUsageRole = Qt::UserRole + 1;
}

AbstractThirdPartyLicensesModel::Item::Item(const ThirdPartyLicenseUsage& usage, const QString& text) :
    QStandardItem(text),
    _usage(usage)
{
}

QVariant AbstractThirdPartyLicensesModel::Item::data(int role) const
{
    if (role == LicenseUsageRole)
        return QVariant::fromValue(_usage);

    return QStandardItem::data(role);
}

const ThirdPartyLicenseUsage& AbstractThirdPartyLicensesModel::Item::getUsage() const
{
    return _usage;
}

QVariant AbstractThirdPartyLicensesModel::NameItem::headerData(Qt::Orientation, int role)
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return "Dependency";
    if (role == Qt::ToolTipRole)
        return "Third-party dependency name";
    return {};
}

QVariant AbstractThirdPartyLicensesModel::LicenseItem::headerData(Qt::Orientation, int role)
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return "License";
    if (role == Qt::ToolTipRole)
        return "Third-party dependency license";
    return {};
}

QVariant AbstractThirdPartyLicensesModel::CoreItem::headerData(Qt::Orientation, int role)
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return "Core";
    if (role == Qt::ToolTipRole)
        return "Whether the dependency is used by ManiVault Core";
    return {};
}

QVariant AbstractThirdPartyLicensesModel::PluginsItem::headerData(Qt::Orientation, int role)
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return "Used by plugins";
    if (role == Qt::ToolTipRole)
        return "Plugins using the dependency";
    return {};
}

AbstractThirdPartyLicensesModel::Row::Row(const ThirdPartyLicenseUsage& usage) : QList<QStandardItem*>()
{
    append(new NameItem(usage, usage.license.name));
    append(new LicenseItem(usage, usage.license.license));
    append(new CoreItem(usage, usage.isCore ? "Yes" : "No"));
    append(new PluginsItem(usage, usage.availablePlugins.join(", ")));
}

AbstractThirdPartyLicensesModel::AbstractThirdPartyLicensesModel(QObject* parent) :
    StandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));
}

void AbstractThirdPartyLicensesModel::setLicenseUsages(const ThirdPartyLicenseUsages& usages)
{
    setRowCount(0);

    for (const auto& usage : usages)
        appendRow(Row(usage));
}

ThirdPartyLicenseUsages AbstractThirdPartyLicensesModel::getLicenseUsages() const
{
    ThirdPartyLicenseUsages usages;

    for (int row = 0; row < rowCount(); ++row)
        usages.push_back(getLicenseUsage(index(row, 0)));

    return usages;
}

ThirdPartyLicenseUsage AbstractThirdPartyLicensesModel::getLicenseUsage(const QModelIndex& index) const
{
    const auto item = dynamic_cast<Item*>(itemFromIndex(index.siblingAtColumn(static_cast<int>(Column::Name))));
    return item ? item->getUsage() : ThirdPartyLicenseUsage();
}

QVariant AbstractThirdPartyLicensesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (static_cast<Column>(section)) {
        case Column::Name: return NameItem::headerData(orientation, role);
        case Column::License: return LicenseItem::headerData(orientation, role);
        case Column::Core: return CoreItem::headerData(orientation, role);
        case Column::Plugins: return PluginsItem::headerData(orientation, role);
        default: return {};
    }
}

}
