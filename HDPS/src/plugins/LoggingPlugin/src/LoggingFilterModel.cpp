#include "LoggingFilterModel.h"

#include <QDebug>

LoggingFilterModel::LoggingFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent)
{
}

bool LoggingFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    return true;// filterName(sourceModel()->index(row, ActionsModel::Column::Name)) && filterType(sourceModel()->index(row, ActionsModel::Column::Type)) && filterScope(sourceModel()->index(row, ActionsModel::Column::IsPublic));
}

bool LoggingFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    //const auto lhsColumn    = static_cast<ActionsModel::Column>(lhs.column());
    //const auto rhsColumn    = static_cast<ActionsModel::Column>(rhs.column());
    //const auto lhsData      = sourceModel()->data(lhs, Qt::EditRole);
    //const auto rhsData      = sourceModel()->data(rhs, Qt::EditRole);

    //if (lhsColumn != rhsColumn)
    //    return false;

    //switch (lhsColumn) {
    //    case ActionsModel::Column::Name:
    //    case ActionsModel::Column::Type:
    //        return lhsData.toString() < rhsData.toString();

    //    case ActionsModel::Column::IsPublic:
    //        return lhsData.toBool() < rhsData.toBool();

    //    case ActionsModel::Column::NumberOfConnections:
    //        return lhsData.toInt() < rhsData.toInt();

    //    default:
    //        break;
    //}

    return false;
}
