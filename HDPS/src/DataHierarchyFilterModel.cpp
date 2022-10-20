#include "DataHierarchyFilterModel.h"
#include "DataHierarchyModelItem.h"

#include <QDebug>

using namespace hdps;

DataHierarchyFilterModel::DataHierarchyFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent)
{
    setRecursiveFilteringEnabled(true);
}

bool DataHierarchyFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    auto modelItem = static_cast<DataHierarchyModelItem*>(index.internalPointer());

    if (!modelItem->isVisible())
        return false;

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index, filterRole()).toString();
        return key.contains(filterRegularExpression());
    }
       
    return false;
}
