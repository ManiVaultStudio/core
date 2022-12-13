#include "ActionsFilterModel.h"
#include "ActionsModel.h"

#include <QDebug>

namespace hdps
{

ActionsFilterModel::ActionsFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent),
    _typeFilterAction(this, "Type"),
    _scopeFilterAction(this, "Scope", { "Public", "Private" }, { "Public" })
{
    setRecursiveFilteringEnabled(true);
}

bool ActionsFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index.siblingAtColumn(filterKeyColumn()), filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }

    return true;
}

bool ActionsFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return lhs.data().toString() < rhs.data().toString();
}

}
