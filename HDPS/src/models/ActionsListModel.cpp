// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ActionsListModel.h"
#include "AbstractActionsManager.h"

using namespace hdps::gui;

#ifdef _DEBUG
    #define ACTIONS_LIST_MODEL_VERBOSE
#endif

namespace hdps
{

ActionsListModel::ActionsListModel(QObject* parent, gui::WidgetAction* rootAction /*= nullptr*/) :
    AbstractActionsModel(parent),
    _rootAction(rootAction)
{
    initialize();
}

void ActionsListModel::initialize()
{
    if (_rootAction) {
        addAction(_rootAction);
    }
    else {
        for (auto action : hdps::actions().getActions())
            addAction(action);
    }
}

WidgetAction* ActionsListModel::getRootAction()
{
    return _rootAction;
}

void ActionsListModel::setRootAction(gui::WidgetAction* rootAction)
{
    if (rootAction == _rootAction)
        return;

    _rootAction = rootAction;

    initialize();

    emit rootActionChanged(_rootAction);
}

void ActionsListModel::actionAddedToManager(gui::WidgetAction* action)
{
    addAction(action);
}

void ActionsListModel::actionAboutToBeRemovedFromManager(gui::WidgetAction* action)
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

void ActionsListModel::addAction(gui::WidgetAction* action)
{
    if (_rootAction) {
        for (auto child : action->children()) {
            auto childAction = dynamic_cast<WidgetAction*>(child);

            if (!childAction)
                continue;

            addAction(childAction);
        }

        appendRow(Row(action));
    }
    else {
        appendRow(Row(action));
    }
}

}
