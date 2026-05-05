// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractWorkflowMessagesModel.h"

using namespace mv;

#ifdef _DEBUG
    #define WORKFLOW_MESSAGES_MODEL_VERBOSE
#endif

using namespace mv::util;

namespace mv {

AbstractWorkflowMessagesModel::Item::Item(WorkflowMessage message) :
    _workflowMessage(std::move(message))
{
    setEditable(false);
}

const WorkflowMessage& AbstractWorkflowMessagesModel::Item::getWorkflowMessage() const
{
    return _workflowMessage;
}

QVariant AbstractWorkflowMessagesModel::LevelItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return static_cast<std::int32_t>(getWorkflowMessage()._level);

        case Qt::DisplayRole:
            return {};

        case Qt::ToolTipRole:
            return QString("%1").arg(data(Qt::DisplayRole).toString());

        case Qt::DecorationRole:
        {
	        switch (getWorkflowMessage()._level)
	        {
	        case WorkflowMessageLevel::Info:
		        return StyledIcon("circle-info").withColor(QColor(220, 235, 255));

	        case WorkflowMessageLevel::Warning:
		        return StyledIcon("circle-exclamation").withColor(QColor(196, 162, 26));

	        case WorkflowMessageLevel::Error:
		        return StyledIcon("triangle-exclamation").withColor(QColor(196, 98, 30));

	        case WorkflowMessageLevel::Critical:
		        return StyledIcon("triangle-exclamation").withColor(QColor(196, 32, 32));
	        }
        }

	    case Qt::TextAlignmentRole:
	        return Qt::AlignLeft;

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractWorkflowMessagesModel::TextItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getWorkflowMessage()._text;

        case Qt::ToolTipRole:
            return QString("%1").arg(data(Qt::DisplayRole).toString());

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractWorkflowMessagesModel::SourceItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
	    case Qt::EditRole:
	    case Qt::DisplayRole:
	        return getWorkflowMessage()._source;

	    case Qt::ToolTipRole:
	        return QString("%1").arg(data(Qt::DisplayRole).toString());
    }

    return Item::data(role);
}

QVariant AbstractWorkflowMessagesModel::TimeStampItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
	    case Qt::EditRole:
	        return getWorkflowMessage()._timestamp;

	    case Qt::DisplayRole:
            return QLocale().toString(data(Qt::EditRole).toDateTime(), QLocale::ShortFormat);

	    case Qt::ToolTipRole:
	        return QString("%1").arg(data(Qt::DisplayRole).toString());

	    default:
	        break;
    }

    return Item::data(role);
}

AbstractWorkflowMessagesModel::AbstractWorkflowMessagesModel(QObject* parent) :
    StandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));
}

QVariant AbstractWorkflowMessagesModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    switch (static_cast<Column>(section))
    {
        case Column::Level:
            return LevelItem::headerData(orientation, role);

        case Column::Source:
            return SourceItem::headerData(orientation, role);

        case Column::Text:
            return TextItem::headerData(orientation, role);

        case Column::DateTime:
            return TimeStampItem::headerData(orientation, role);

        case Column::Count:
            break;

        default:
            break;
    }

    return {};
}

}