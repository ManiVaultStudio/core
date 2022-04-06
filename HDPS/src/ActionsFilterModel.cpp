#include "ActionsFilterModel.h"
#include "ActionsModel.h"

#include <QDebug>

namespace hdps
{

ActionsFilterModel::ActionsFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent),
    _typeFilter(""),
    _scopeFilter(ScopeFilter::None)
{
}

bool ActionsFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    return filterName(sourceModel()->index(row, ActionsModel::Column::Name)) && filterType(sourceModel()->index(row, ActionsModel::Column::Type)) && filterScope(sourceModel()->index(row, ActionsModel::Column::IsPublic));
}

bool ActionsFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    const auto lhsColumn    = static_cast<ActionsModel::Column>(lhs.column());
    const auto rhsColumn    = static_cast<ActionsModel::Column>(rhs.column());
    const auto lhsData      = sourceModel()->data(lhs, Qt::EditRole);
    const auto rhsData      = sourceModel()->data(rhs, Qt::EditRole);

    if (lhsColumn != rhsColumn)
        return false;

    switch (lhsColumn) {
        case ActionsModel::Column::Name:
        case ActionsModel::Column::Type:
            return lhsData.toString() < rhsData.toString();

        case ActionsModel::Column::IsPublic:
            return lhsData.toBool() < rhsData.toBool();

        case ActionsModel::Column::NumberOfConnections:
            return lhsData.toInt() < rhsData.toInt();

        default:
            break;
    }

    return false;
}

QString ActionsFilterModel::getNameFilter() const
{
    return _nameFilter;
}

void ActionsFilterModel::setNameFilter(const QString& nameFilter)
{
    if (nameFilter == _nameFilter)
        return;

    _nameFilter = nameFilter;

    invalidateFilter();
}

QString ActionsFilterModel::getTypeFilter() const
{
    return _typeFilter;
}

void ActionsFilterModel::setTypeFilter(const QString& typeFilter)
{
    if (typeFilter == _typeFilter)
        return;

    _typeFilter = typeFilter;

    invalidateFilter();
}

ActionsFilterModel::ScopeFilter ActionsFilterModel::getScopeFilter() const
{
    return _scopeFilter;
}

void ActionsFilterModel::setScopeFilter(const ScopeFilter& scopeFilter)
{
    if (scopeFilter == _scopeFilter)
        return;

    _scopeFilter = scopeFilter;

    invalidateFilter();
}

bool ActionsFilterModel::filterName(const QModelIndex& index) const
{
    const auto actionName = sourceModel()->data(index, Qt::EditRole).toString();

    if (_nameFilter.isEmpty())
        return true;
    else
        return actionName.contains(_nameFilter, Qt::CaseInsensitive);

    return false;
}

bool ActionsFilterModel::filterType(const QModelIndex& index) const
{
    const auto actionType = sourceModel()->data(index, Qt::EditRole).toString();

    if (_typeFilter.isEmpty())
        return true;
    else
        return actionType == _typeFilter;

    return false;
}

bool ActionsFilterModel::filterScope(const QModelIndex& index) const
{
    const auto actionIsPublic = sourceModel()->data(index, Qt::EditRole).toBool();

    if (_scopeFilter & ScopeFilter::None) {
        return true;
    }
    else {
        if ((_scopeFilter & ScopeFilter::Public) && actionIsPublic)
            return true;

        if ((_scopeFilter & ScopeFilter::Private) && !actionIsPublic)
            return true;
    }

    return false;
}

}
