// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowMessagesFilterModel.h"

#include "util/WorkflowMessage.h"

#ifdef _DEBUG
    #define WORKFLOW_MESSAGES_FILTER_MODEL_VERBOSE
#endif

using namespace mv::util;

namespace mv {

	WorkflowMessagesFilterModel::WorkflowMessagesFilterModel(QObject* parent /*= nullptr*/) :
    SortFilterProxyModel(parent),
    _filterLevelAction(this, "Filter level", severityLevelNames.values(), {
		getSeverityLevelName(SeverityLevel::Warning),
        getSeverityLevelName(SeverityLevel::Error),
        getSeverityLevelName(SeverityLevel::Critical)
    })
{
    connect(&_filterLevelAction, &gui::OptionsAction::selectedOptionsChanged, this, &WorkflowMessagesFilterModel::invalidateFilter);

    _filterLevelAction.setStretch(2);
}

bool WorkflowMessagesFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index.siblingAtColumn(filterKeyColumn()), filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }

    const auto levelIndex   = index.siblingAtColumn(static_cast<int>(AbstractWorkflowMessagesModel::Column::Level)).data(Qt::EditRole).toInt();
    const auto levelName    = getSeverityLevelName(static_cast<SeverityLevel>(levelIndex));

    if (!_filterLevelAction.hasSelectedOptions())
        return false;

    if (!_filterLevelAction.isOptionSelected(levelName))
        return false;

    return true;
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

        case static_cast<int>(AbstractWorkflowMessagesModel::Column::Source): {
            const auto lhsSource = lhs.data(Qt::EditRole).toString();
            const auto rhsSource = rhs.data(Qt::EditRole).toString();

            if (lhsSource != rhsSource)
                return lhsSource < rhsSource;

            break;
        }

        case static_cast<int>(AbstractWorkflowMessagesModel::Column::Text): {
            const auto lhsText = lhs.data(Qt::EditRole).toString();
            const auto rhsText = rhs.data(Qt::EditRole).toString();

				if (lhsText != rhsText)
                return lhsText < rhsText;
			
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

}
