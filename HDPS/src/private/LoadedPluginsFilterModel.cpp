#include "LoadedPluginsFilterModel.h"
#include "LoadedPluginsModel.h"

#include <QDebug>

LoadedPluginsFilterModel::LoadedPluginsFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent)
{
}

bool LoadedPluginsFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    return true;
}

bool LoadedPluginsFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return false;
}
