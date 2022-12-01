#include "LoggingFilterModel.h"
#include "LoggingModel.h"

#include <QDebug>

LoggingFilterModel::LoggingFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent)
{
}

bool LoggingFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index.siblingAtColumn(static_cast<std::int32_t>(LoggingModel::Column::Message)), filterRole()).toString();
        return key.contains(filterRegularExpression());
    }

    return false;
}

bool LoggingFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    const auto lhsColumn    = static_cast<LoggingModel::Column>(lhs.column());
    const auto rhsColumn    = static_cast<LoggingModel::Column>(rhs.column());
    const auto lhsData      = sourceModel()->data(lhs, Qt::EditRole);
    const auto rhsData      = sourceModel()->data(rhs, Qt::EditRole);

    if (lhsColumn != rhsColumn)
        return false;

    switch (lhsColumn) {
        case LoggingModel::Column::Number:
            return lhsData.toLongLong() < rhsData.toLongLong();

        case LoggingModel::Column::Category:
            return lhsData.toString() < rhsData.toString();

        case LoggingModel::Column::Type:
            return lhsData.toString() < rhsData.toString();

        case LoggingModel::Column::FileAndLine:
            return lhsData.toString() < rhsData.toString();

        case LoggingModel::Column::Function:
            return lhsData.toString() < rhsData.toString();

        case LoggingModel::Column::Message:
            return lhsData.toString() < rhsData.toString();

        default:
            break;
    }

    return false;
}
