#include "ActionsFilterModel.h"
#include "ActionsModel.h"
#include "CoreInterface.h"

#include <QDebug>

using namespace hdps::gui;

namespace hdps
{

ActionsFilterModel::ActionsFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent),
    _typeFilterAction(this, "Type"),
    _typeCompleter(this),
    _scopeFilterAction(this, "Scope", { "Private", "Public" }, { "Private", "Public" }),
    _connectedFilterAction(this, "Connected", true, true)
{
    setRecursiveFilteringEnabled(true);

    _typeFilterAction.setClearable(true);
    _typeFilterAction.setCompleter(&_typeCompleter);

    _scopeFilterAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);

    connect(&_typeFilterAction, &StringAction::stringChanged, this, &ActionsFilterModel::invalidate);
    connect(&_scopeFilterAction, &OptionsAction::selectedOptionsChanged, this, &ActionsFilterModel::invalidate);
    connect(&_connectedFilterAction, &ToggleAction::toggled, this, &ActionsFilterModel::invalidate);

    connect(&actions().getActionsModel(), &ActionsModel::actionTypesChanged, this, [this](const QStringList& actionTypes) -> void {
        _typeFilterAction.getCompleter()->setModel(new QStringListModel(actionTypes));
    });
}

bool ActionsFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    return true;
    if (!index.isValid())
        return true;

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index.siblingAtColumn(filterKeyColumn()), filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }

    const auto typeFilter   = _typeFilterAction.getString();

    if (!typeFilter.isEmpty()) {
        const auto type = sourceModel()->data(index.siblingAtColumn(static_cast<int>(ActionsModel::Column::Type)), Qt::EditRole).toString();

        if (type != typeFilter)
            return false;
    }

    const auto isConnected = sourceModel()->data(index.siblingAtColumn(static_cast<int>(ActionsModel::Column::Connected)), Qt::EditRole).toBool();

    if (_connectedFilterAction.isChecked() && !isConnected)
        return false;

    //if (!parent.isValid()) {
        const auto scope = sourceModel()->data(index.siblingAtColumn(static_cast<int>(ActionsModel::Column::Scope)), Qt::EditRole).toInt();

        if (scope == 0 && !_scopeFilterAction.getSelectedOptionIndices().contains(0))
            return false;

        if (scope == 1 && !_scopeFilterAction.getSelectedOptionIndices().contains(1))
            return false;
    //}

    return true;
}

bool ActionsFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return lhs.data().toString() < rhs.data().toString();
}

WidgetAction* ActionsFilterModel::getAction(std::int32_t rowIndex)
{
    const auto sourceModelIndex = mapToSource(index(rowIndex, 0));
    return static_cast<ActionsModel*>(sourceModel())->getAction(sourceModelIndex.row());
}

}
