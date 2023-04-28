#include "ActionsHierarchyModel.h"
#include "AbstractActionsManager.h"

using namespace hdps::gui;

#ifdef _DEBUG
    #define ACTIONS_HIERARCHY_MODEL_VERBOSE
#endif

namespace hdps
{

ActionsHierarchyModel::ActionsHierarchyModel(QObject* parent, gui::WidgetAction* rootAction /*= nullptr*/) :
    ActionsModel(parent),
    _rootAction(rootAction)
{
    Q_ASSERT(_rootAction != nullptr);

    initialize();
}

void ActionsHierarchyModel::initialize()
{
    setRowCount(0);

    addAction(_rootAction);
}

void ActionsHierarchyModel::addAction(gui::WidgetAction* action)
{
    if (action == _rootAction)
        appendRow(Row(action));
    else {
        auto parentAction = action->getParentAction();

        if (parentAction) {
            const auto parentActionIndex = getActionIndex(parentAction);

            if (parentActionIndex.isValid())
                itemFromIndex(parentActionIndex.siblingAtColumn(static_cast<int>(Column::Name)))->appendRow(Row(action));

        }
    }

    for (auto child : action->children()) {
        auto childAction = dynamic_cast<WidgetAction*>(child);

        if (!childAction)
            continue;

        addAction(childAction);
    }
}

void ActionsHierarchyModel::removeAction(gui::WidgetAction* action)
{
    const auto matches = match(index(0, static_cast<int>(Column::ID), QModelIndex()), Qt::EditRole, action->getId(), -1, Qt::MatchFlag::MatchRecursive);

    if (matches.isEmpty())
        return;

    QList<QPersistentModelIndex> persistentMatches;

    for (const auto& match : matches)
        persistentMatches << QPersistentModelIndex(match);

    for (const auto& persistentMatch : persistentMatches)
        removeRow(persistentMatch.row(), persistentMatch.parent());
}

}