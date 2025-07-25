// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractPageActionsModel.h"

#include <QStandardItem>

#ifdef _DEBUG
    //#define ABSTRACT_PAGE_ACTIONS_MODEL_VERBOSE
#endif

using namespace mv;
using namespace mv::gui;

AbstractPageActionsModel::Item::Item(const PageActionSharedPtr& pageAction, bool editable) :
	_pageAction(pageAction)
{
    setEditable(editable);
}

QVariant AbstractPageActionsModel::Item::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
	        return QVariant::fromValue(getPageAction()->getTitle());

	    case Qt::DisplayRole:
	        return "";

        case Qt::ToolTipRole:
            return getPageAction()->getTooltip();

	    default:
	        break;
    }

	return QStandardItem::data(role);
}

PageActionSharedPtr AbstractPageActionsModel::Item::getPageAction() const
{
    return _pageAction;
}

AbstractPageActionsModel::AbstractPageActionsModel(QObject* parent /*= nullptr*/) :
    StandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));
}

void AbstractPageActionsModel::reset()
{
    removeRows(0, rowCount());

    emit layoutChanged();
}
