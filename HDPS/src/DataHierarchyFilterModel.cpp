#include "DataHierarchyFilterModel.h"

#include <QDebug>

DataHierarchyFilterModel::DataHierarchyFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent)
{
    setRecursiveFilteringEnabled(true);
}

bool DataHierarchyFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    QString key = sourceModel()->data(sourceModel()->index(row, 0, parent), filterRole()).toString();
    return key.contains(filterRegularExpression());
}
