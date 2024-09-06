// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginsFilterModel.h"
#include "AbstractPluginsModel.h"

#include <QDebug>

#ifdef _DEBUG
    #define PLUGINS_FILTER_MODEL_VERBOSE
#endif

using namespace mv::gui;

namespace mv {

PluginsFilterModel::PluginsFilterModel(QObject* parent /*= nullptr*/) :
    SortFilterProxyModel(parent),
    _useFilterFunctionAction(this, "Use filter function", false),
    _instantiatedPluginsOnlyAction(this, "Show only instantiated plugins", true)
{
    _instantiatedPluginsOnlyAction.setToolTip("Show only instantiated plugins are all available plugins");

    setRecursiveFilteringEnabled(true);

    connect(&_instantiatedPluginsOnlyAction, &ToggleAction::toggled, this, &PluginsFilterModel::invalidate);

    invalidate();
}

bool PluginsFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    if (_instantiatedPluginsOnlyAction.isChecked()) {
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

    auto abstractPluginsModel = dynamic_cast<AbstractPluginsModel*>(sourceModel());

    auto plugin = dynamic_cast<AbstractPluginsModel::Item*>(abstractPluginsModel->itemFromIndex(index))->getPlugin();

    if (_useFilterFunctionAction.isChecked() && _filterFunction)
        return _filterFunction(plugin);

    if (!_filterPluginTypes.isEmpty())
        return _filterPluginTypes.contains(plugin->getType());

    return true;
}

bool PluginsFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return lhs.data().toString() < rhs.data().toString();
}

void PluginsFilterModel::setFilterFunction(const FilterFunction& filterFunction)
{
    _filterFunction = filterFunction;

    invalidate();
}

plugin::Types PluginsFilterModel::getFilterPluginTypes() const
{
    return _filterPluginTypes;
}

void PluginsFilterModel::setFilterPluginTypes(const plugin::Types& filterPluginTypes)
{
    _filterPluginTypes = filterPluginTypes;

    invalidate();
}

bool PluginsFilterModel::hasPluginInstances(const QModelIndex& index, int level /*= 0*/) const
{
    const auto numberOfRows = sourceModel()->rowCount(index);

#ifdef PLUGINS_FILTER_MODEL_VERBOSE
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

}