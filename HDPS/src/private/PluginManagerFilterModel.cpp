#include "PluginManagerFilterModel.h"
#include "PluginManagerModel.h"

#include <QDebug>

#ifdef _DEBUG
    #define PLUGIN_MANAGER_FILTER_MODEL_VERBOSE
#endif

PluginManagerFilterModel::PluginManagerFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent)
{
}

bool PluginManagerFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    return true;
}

bool PluginManagerFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return false;
}
