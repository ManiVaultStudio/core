#include "ActionHierarchyFilterModel.h"
#include "ActionHierarchyModelItem.h"

#include <QDebug>

using namespace hdps;

ActionHierarchyFilterModel::ActionHierarchyFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent)
{
    setRecursiveFilteringEnabled(true);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

bool ActionHierarchyFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    auto modelItem = static_cast<ActionHierarchyModelItem*>(index.internalPointer());

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index, filterRole()).toString();
        return key.contains(filterRegularExpression());
    }
       
    return false;
}