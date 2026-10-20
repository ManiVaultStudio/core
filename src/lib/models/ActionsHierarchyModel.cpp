// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ActionsHierarchyModel.h"

using namespace mv::gui;

#ifdef _DEBUG
    #define ACTIONS_HIERARCHY_MODEL_VERBOSE
#endif

namespace mv
{

ActionsHierarchyModel::ActionsHierarchyModel(QObject* parent, gui::WidgetAction* rootAction /*= nullptr*/) :
    AbstractActionsModel(parent),
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

void ActionsHierarchyModel::actionAddedToManager(gui::WidgetAction* action)
{

}

void ActionsHierarchyModel::actionAboutToBeRemovedFromManager(gui::WidgetAction* action)
{

}

void ActionsHierarchyModel::publicActionAddedToManager(gui::WidgetAction* publicAction)
{

}

void ActionsHierarchyModel::publicActionAboutToBeRemovedFromManager(gui::WidgetAction* publicAction)
{

}

void ActionsHierarchyModel::addAction(gui::WidgetAction* action)
{
    if (action == _rootAction)
        appendRow(Row(action));
    else {
        auto parentAction = action->getParent();

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

//    setColumnCount(18);
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