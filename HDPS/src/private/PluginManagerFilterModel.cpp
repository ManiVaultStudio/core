#include "PluginManagerFilterModel.h"
#include "PluginManagerModel.h"

#include <QDebug>

#ifdef _DEBUG
    #define PLUGIN_MANAGER_FILTER_MODEL_VERBOSE
#endif

using namespace hdps::gui;

PluginManagerFilterModel::PluginManagerFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent),
    _showOnlyLoadedAction(this, "Show only loaded plugins", false, false)
{
    _showOnlyLoadedAction.setConnectionPermissionsToNone();

    setRecursiveFilteringEnabled(true);

    connect(&_showOnlyLoadedAction, &ToggleAction::toggled, this, &PluginManagerFilterModel::invalidate);
}

bool PluginManagerFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    if (_showOnlyLoadedAction.isChecked()) {
        if (!index.parent().isValid()) {
            if (!hasPluginInstances(index))
                return false;
        }

        if (index.parent().isValid() && !index.parent().parent().isValid()) {
            if (!hasPluginInstances(index, 1))
                return false;
        }
    }

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index.siblingAtColumn(filterKeyColumn()), filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }

    return true;
}

bool PluginManagerFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return lhs.data().toString() < rhs.data().toString();
}

bool PluginManagerFilterModel::hasPluginInstances(const QModelIndex& index, int level /*= 0*/) const
{
    const auto numberOfRows = sourceModel()->rowCount(index);

#ifdef PLUGIN_MANAGER_FILTER_MODEL_VERBOSE
    qDebug() << __FUNCTION__ << index.data().toString() << "level" << level << "number of children" << numberOfRows;
#endif

    if (level == 2 && sourceModel()->rowCount(index.parent()) >= 1)
        return true;

    for (int rowIndex = 0; rowIndex < numberOfRows; rowIndex++) {
        if (hasPluginInstances(sourceModel()->index(rowIndex, 0, index), level + 1))
            return true;
    }

    return false;
}