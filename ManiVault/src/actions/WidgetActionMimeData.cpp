// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionMimeData.h"

#include <set>

namespace mv::gui {

WidgetActionMimeData::WidgetActionMimeData(WidgetAction* action) :
    _action(action)
{
    ActionsFilterModel  actionsFilterModel;

    actionsFilterModel.setSourceModel(& mv::actions().getActionsListModel());
    actionsFilterModel.getScopeFilterAction().setSelectedOptions({ "Private" });

    QStringList actionTypes { getAction()->getTypeString() };

    for (const auto& child : getAction()->getChildren(true))
        actionTypes << child->getTypeString();

    actionTypes.removeDuplicates();

    qDebug() << actionTypes;

    actionsFilterModel.getTypeFilterAction().setStrings(actionTypes);

    for (int rowIndex = 0; rowIndex < actionsFilterModel.rowCount(); ++rowIndex) {
        auto candidateAction = actionsFilterModel.getAction(rowIndex);

        const auto connections = AbstractActionsManager::getCandidateConnectionsForDescendants(getAction(), candidateAction);

        //qDebug() << candidateAction->text() << connections.size();

        //if (connections.empty())
        //    continue;

        if (candidateAction == getAction())
            continue;

        if (!candidateAction->isEnabled())
            continue;

        if (!candidateAction->mayConnect(WidgetAction::Gui))
            continue;

        if (candidateAction->isHighlightVisible())
            candidateAction->unHighlight();

        if (getAction()->getTypeString() != candidateAction->getTypeString()) {
            if (connections.empty())
                continue;

        	candidateAction->setHighlightMode(WidgetAction::HighlightMode::Light);
        	candidateAction->setHighlightDescription(QString("%1 indirect connection(1)").arg(QString::number(connections.size())));
        } else {
            candidateAction->setHighlightMode(WidgetAction::HighlightMode::Moderate);
        }

    	if (candidateAction->isConnected() && (candidateAction->getPublicAction() == getAction()))
            continue;

        //candidateAction->highlight();
        _highlightActions << candidateAction;
    }
    
    qDebug() << "Highlighting" << _highlightActions.count();

    for (auto highlightAction : _highlightActions)
        highlightAction->highlight();
}

WidgetActionMimeData::~WidgetActionMimeData()
{
    for (auto highlightAction : _highlightActions)
        highlightAction->unHighlight();
}

QStringList WidgetActionMimeData::formats() const
{
    return { format() };
}

}
