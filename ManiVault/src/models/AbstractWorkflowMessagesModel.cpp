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

    return QString(
        "<p><b>Event details</b></p><p>%1</p>"
    ).arg(variantMapToHtml(map));

    return QString(
        "<div style='background:%1; color:%2; padding:6px; border: 1px solid %3;'>"
        "<div style='margin-left:8px; margin-bottom: 1px; font-size:14px; font-weight:bold;'>%4</div><p style='margin-top:0px;'>%5</p>"
        "</div>"
    ).arg(QToolTip::palette().color(QPalette::ToolTipBase).name(), QToolTip::palette().color(QPalette::ToolTipText).name(), QToolTip::palette().color(QPalette::Text).name(), "Event details", variantMapToHtml(map));
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
        {
	        switch (getWorkflowMessage()->level)
	        {
		        case SeverityLevel::Info:
			        return StyledIcon("circle-info").withColor(QColor(220, 235, 255));

		        case SeverityLevel::Warning:
			        return StyledIcon("circle-exclamation").withColor(QColor(196, 162, 26));

		        case SeverityLevel::Error:
			        return StyledIcon("triangle-exclamation").withColor(QColor(196, 98, 30));

		        case SeverityLevel::Fatal:
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
    
    switch (role) {
	    case Qt::EditRole:
            return getWorkflowMessage()->details;

		case Qt::DisplayRole:
            return QString("%1").arg(data(Qt::EditRole).toMap().isEmpty() ? "Not available..." : data(Qt::EditRole).toMap().keys().join(", "));

        case Qt::ToolTipRole:
            return getTooltip();

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

        default:
            break;
    }

    return {};
}

}
