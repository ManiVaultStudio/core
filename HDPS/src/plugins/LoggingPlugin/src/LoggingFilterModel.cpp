// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LoggingFilterModel.h"
#include "LoggingModel.h"

#include <util/Logger.h>

#include <QDebug>

using namespace hdps::util;
using namespace hdps::gui;

LoggingFilterModel::LoggingFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent),
    _filterTypeAction(this, "Filter type", Logger::messageTypeNames.values(), Logger::messageTypeNames.values())
{
    setFilterKeyColumn(static_cast<int>(LoggingModel::Column::Message));

    _filterTypeAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);

    connect(&_filterTypeAction, &OptionsAction::selectedOptionsChanged, this, &LoggingFilterModel::invalidate);
}

bool LoggingFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    const auto* messageRecord = static_cast<MessageRecord*>(index.internalPointer());

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index.siblingAtColumn(filterKeyColumn()), filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }

    const auto selectedFilterTypeOptions = _filterTypeAction.getSelectedOptions();

    switch (messageRecord->type)
    {
        case QtMsgType::QtDebugMsg:
        {
            if (!selectedFilterTypeOptions.contains("Debug"))
                return false;

            break;
        }

        case QtMsgType::QtWarningMsg:
        {
            if (!selectedFilterTypeOptions.contains("Warning"))
                return false;

            break;
        }

        case QtMsgType::QtCriticalMsg:
        {
            if (!selectedFilterTypeOptions.contains("Critical"))
                return false;

            break;
        }

        case QtMsgType::QtFatalMsg:
        {
            if (!selectedFilterTypeOptions.contains("Fatal"))
                return false;

            break;
        }

        case QtMsgType::QtInfoMsg:
        {
            if (!selectedFilterTypeOptions.contains("Info"))
                return false;

            break;
        }

        default:
            break;
    }

    return true;
}

bool LoggingFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    const auto lhsColumn    = static_cast<LoggingModel::Column>(lhs.column());
    const auto rhsColumn    = static_cast<LoggingModel::Column>(rhs.column());
    const auto lhsData      = sourceModel()->data(lhs, Qt::EditRole);
    const auto rhsData      = sourceModel()->data(rhs, Qt::EditRole);

    if (lhsColumn != rhsColumn)
        return false;

    switch (lhsColumn) {
        case LoggingModel::Column::Number:
            return lhsData.toLongLong() < rhsData.toLongLong();

        case LoggingModel::Column::Category:
            return lhsData.toString() < rhsData.toString();

        case LoggingModel::Column::Type:
            return lhsData.toString() < rhsData.toString();

        case LoggingModel::Column::FileAndLine:
            return lhsData.toString() < rhsData.toString();

        case LoggingModel::Column::Function:
            return lhsData.toString() < rhsData.toString();

        case LoggingModel::Column::Message:
            return lhsData.toString() < rhsData.toString();

        default:
            break;
    }

    return false;
}
