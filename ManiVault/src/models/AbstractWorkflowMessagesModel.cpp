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

const util::WorkflowMessage& AbstractWorkflowMessagesModel::Item::getWorkflowMessage() const
{
    return _workflowMessage;
}

QVariant AbstractWorkflowMessagesModel::LevelItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        //case Qt::EditRole:
        //    return static_cast<std::int32_t>(getWorkflowMessage().getLevel());

        //case Qt::DisplayRole:
        //    return Script::getTypeName(getScript()->getType());

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
        //case Qt::EditRole:
        //    return static_cast<std::int32_t>(getWorkflowMessage().getSource());

        //case Qt::DisplayRole:
        //    return Script::getLanguageName(getScript()->getLanguage());

        case Qt::ToolTipRole:
            return QString("%1").arg(data(Qt::DisplayRole).toString());

        case Qt::DecorationRole:
        {
            //switch (getScript()->getLanguage()) {
            //    case Script::Language::Python:
            //        return StyledIcon("python");

            //    case Script::Language::R:
            //        return StyledIcon("r");

            //    default:
            //        return QIcon();
            //}
        }

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

QVariant AbstractWorkflowMessagesModel::DateTimeItem::data(int role /*= Qt::UserRole + 1*/) const
{
    //switch (role) {
	   // case Qt::EditRole:
	   // case Qt::DisplayRole:
	   //     return getWorkflowMessage()._dateTime;

	   // case Qt::ToolTipRole:
	   //     return QString("%1").arg(data(Qt::DisplayRole).toString());

	   // default:
	   //     break;
    //}

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
            return DateTimeItem::headerData(orientation, role);

        default:
            break;
    }

    return {};
}

}