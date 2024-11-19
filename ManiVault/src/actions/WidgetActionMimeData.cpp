// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionMimeData.h"

namespace mv::gui {

WidgetActionMimeData::WidgetActionMimeData(WidgetAction* action) :
    QMimeData(),
    _action(action),
    _highlightActions()
{
    ActionsListModel    actionsListModel;      /** Actions list model */
    ActionsFilterModel  actionsFilterModel;    /** Filtered actions model */

    actionsFilterModel.setSourceModel(&actionsListModel);
    actionsFilterModel.getScopeFilterAction().setSelectedOptions({ "Private" });
    actionsFilterModel.getTypeFilterAction().setString(getAction()->getTypeString());

    for (int rowIndex = 0; rowIndex < actionsFilterModel.rowCount(); ++rowIndex) {
        auto candidateAction = actionsFilterModel.getAction(rowIndex);

        if (candidateAction == getAction())
            continue;

        if (!candidateAction->isEnabled())
            continue;

        if (!candidateAction->mayConnect(WidgetAction::Gui))
            continue;

        if (candidateAction->isHighlighted())
            candidateAction->unHighlight();

        if (candidateAction->isConnected() && (candidateAction->getPublicAction() == getAction()))
            continue;

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