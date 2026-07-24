// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractWorkflowMessagesModel.h"

#include "util/Serialization.h"
#include "util/Miscellaneous.h"

#include <QFontDatabase>
#include <QToolTip>

#ifdef _DEBUG
    #define WORKFLOW_MESSAGES_MODEL_VERBOSE
#endif

using namespace mv;
using namespace mv::util;
using namespace mv::workflow;

namespace mv {

AbstractWorkflowMessagesModel::Item::Item(SharedWorkflowMessage message) :
    _workflowMessage(std::move(message))
{
    setEditable(false);
}

SharedWorkflowMessage AbstractWorkflowMessagesModel::Item::getWorkflowMessage() const
{
    return _workflowMessage;
}

QString AbstractWorkflowMessagesModel::Item::getTooltip() const
{
    const auto map = _workflowMessage->details;

    if (map.isEmpty())
        return {};

    return QString("<p><b>Event details</b></p><p>%1</p>").arg(variantMapToHtml(map));
}

QVariant AbstractWorkflowMessagesModel::LevelItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return static_cast<std::int32_t>(getWorkflowMessage()->level);

        case Qt::DisplayRole:
            return {};

        case Qt::ToolTipRole:
            return getTooltip();

        case Qt::DecorationRole:
	        return getSeverityLevelIcon(getWorkflowMessage()->level);

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
            return getWorkflowMessage()->text;

        case Qt::DisplayRole:
            return data(Qt::EditRole).toString();
            //return QString("%1: %2").arg(getSeverityLevelName(getWorkflowMessage()._level), data(Qt::EditRole).toString());

        case Qt::ToolTipRole:
            return data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractWorkflowMessagesModel::EmitterItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
	    case Qt::EditRole:
	    case Qt::DisplayRole:
	        return getWorkflowMessage()->emitter;

	    case Qt::ToolTipRole:
	        return QString("%1").arg(data(Qt::DisplayRole).toString());
    }

    return Item::data(role);
}

QVariant AbstractWorkflowMessagesModel::LocationItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
	    case Qt::EditRole:
	    case Qt::DisplayRole:
	        return getWorkflowMessage()->location;

	    case Qt::ToolTipRole:
	        return QString("%1").arg(data(Qt::DisplayRole).toString());
    }

    return Item::data(role);
}

QVariant AbstractWorkflowMessagesModel::DetailsItem::data(int role) const
{
    const auto& details = getWorkflowMessage()->details;
    switch (role) {
	    case Qt::EditRole:
	        return details;

	    case Qt::DisplayRole:
	        return "";

        case Qt::DecorationRole:
        {
            return details.isEmpty() ? StyledIcon() : StyledIcon("file-lines");
        }

        case Qt::ToolTipRole:
            return details.isEmpty() ? "No details" : "View diagnostic details";

	    case Qt::FontRole: {
	        QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
	        font.setItalic(true);
	        return font;
    }

    default:
        break;
    }

    return Item::data(role);
}

QVariant AbstractWorkflowMessagesModel::TimeStampItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
	    case Qt::EditRole:
	        return getWorkflowMessage()->timestamp;

	    case Qt::DisplayRole: {
	        const auto dt = getWorkflowMessage()->timestamp;
	        return QLocale().toString(dt.time(), "HH:mm:ss");
	    }

	    case Qt::ToolTipRole:
	        return QString("%1").arg(data(Qt::DisplayRole).toString());

	    default:
	        break;
    }

    return Item::data(role);
}

QVariant AbstractWorkflowMessagesModel::IdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
	    case Qt::EditRole:
	        return getWorkflowMessage()->id;

	    case Qt::DisplayRole:
	        return data(Qt::EditRole).toString();

	    case Qt::ToolTipRole:
	        return data(Qt::DisplayRole).toString();

	    default:
	        break;
    }

    return Item::data(role);
}

QVariant AbstractWorkflowMessagesModel::ParentIdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
	    case Qt::EditRole:
	        return getWorkflowMessage()->parentId;

	    case Qt::DisplayRole:
	        return data(Qt::EditRole).toString();

	    case Qt::ToolTipRole:
	        return data(Qt::DisplayRole).toString();

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

        case Column::Emitter:
            return EmitterItem::headerData(orientation, role);

        case Column::Location:
            return LocationItem::headerData(orientation, role);

        case Column::Text:
            return TextItem::headerData(orientation, role);

        case Column::Details:
            return DetailsItem::headerData(orientation, role);

        case Column::TimeStamp:
            return TimeStampItem::headerData(orientation, role);

        case Column::ID:
            return IdItem::headerData(orientation, role);

        case Column::ParentID:
            return ParentIdItem::headerData(orientation, role);

        default:
            break;
    }

    return {};
}

}
