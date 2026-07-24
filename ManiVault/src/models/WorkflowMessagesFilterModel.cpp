// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowMessagesFilterModel.h"
#include "AbstractWorkflowMessagesModel.h"

#include "workflow/WorkflowMessage.h"

#ifdef _DEBUG
    #define WORKFLOW_MESSAGES_FILTER_MODEL_VERBOSE
#endif

using namespace mv;
using namespace mv::util;
using namespace mv::workflow;

namespace mv {

	WorkflowMessagesFilterModel::WorkflowMessagesFilterModel(QObject* parent /*= nullptr*/) :
    SortFilterProxyModel(parent),
    _filterLevelAction(this, "Filter level", severityLevelNames.values(), {
        getSeverityLevelName(SeverityLevel::Info),
		getSeverityLevelName(SeverityLevel::Warning),
        getSeverityLevelName(SeverityLevel::Error),
        getSeverityLevelName(SeverityLevel::Fatal)
    })
{
	connect(&_filterLevelAction, &gui::OptionsAction::selectedOptionsChanged, this, &WorkflowMessagesFilterModel::invalidateModeFilter);

    _filterLevelAction.setStretch(2);
}

bool WorkflowMessagesFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto* source = sourceModel();

    if (!source)
        return false;

    const auto levelModelIndex = source->index(row, static_cast<int>(AbstractWorkflowMessagesModel::Column::Level), parent);

    if (!levelModelIndex.isValid())
        return false;

    if (filterRegularExpression().isValid() && !filterRegularExpression().pattern().isEmpty()) {
        const auto filterIndex = source->index(row, filterKeyColumn(), parent);

        if (!filterIndex.isValid())
            return false;

        const auto key = source->data(filterIndex, filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }

    const auto* itemModel = qobject_cast<const QStandardItemModel*>(source);

    if (!itemModel)
        return false;

    const auto* item = dynamic_cast<const AbstractWorkflowMessagesModel::Item*>(itemModel->itemFromIndex(levelModelIndex));

    if (!item)
        return false;

    if (!_filterLevelAction.hasSelectedOptions())
        return false;

    const auto levelName = getSeverityLevelName(item->getWorkflowMessage()->level);

    return _filterLevelAction.isOptionSelected(levelName);
}

bool WorkflowMessagesFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
	switch (lhs.column()) {
		case static_cast<int>(AbstractWorkflowMessagesModel::Column::Level): {
            const auto lhsLevel = static_cast<SeverityLevel>(lhs.data(Qt::EditRole).toInt());
            const auto rhsLevel = static_cast<SeverityLevel>(rhs.data(Qt::EditRole).toInt());

			if (lhsLevel != rhsLevel)
                return lhsLevel < rhsLevel;

			break;
        }

        case static_cast<int>(AbstractWorkflowMessagesModel::Column::Emitter):
        case static_cast<int>(AbstractWorkflowMessagesModel::Column::Location):
        case static_cast<int>(AbstractWorkflowMessagesModel::Column::Text): {
            const auto lhsString = lhs.data(Qt::EditRole).toString();
            const auto rhsString = rhs.data(Qt::EditRole).toString();

				if (lhsString != rhsString)
                return lhsString < rhsString;
			
            break;
        }

        case static_cast<int>(AbstractWorkflowMessagesModel::Column::TimeStamp): {
            const auto lhsDateTime = lhs.data(Qt::EditRole).toDateTime();
            const auto rhsDateTime = rhs.data(Qt::EditRole).toDateTime();

			if (lhsDateTime != rhsDateTime)
                return lhsDateTime < rhsDateTime;
			
            break;
        }

        case static_cast<int>(AbstractWorkflowMessagesModel::Column::Details): {
            const auto lhsDetails = lhs.data(Qt::EditRole).toString();
            const auto rhsDetails = rhs.data(Qt::EditRole).toString();

            if (lhsDetails != rhsDetails)
                return lhsDetails < rhsDetails;

            break;
        }
	}

	return SortFilterProxyModel::lessThan(lhs, rhs);
}

void WorkflowMessagesFilterModel::invalidateModeFilter() {
  beginFilterChange();
  endFilterChange(QSortFilterProxyModel::Direction::Rows);
}

}
