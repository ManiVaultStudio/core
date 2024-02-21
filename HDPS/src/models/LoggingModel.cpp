// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LoggingModel.h"

#include "Application.h"

#include "util/Logger.h"

#include <QBrush>
#include <QDateTime>
#include <QFileInfo>
#include <QRegularExpression>
#include <QStringList>
#include <QAbstractEventDispatcher>

#include <algorithm>
#include <cassert>

#ifdef _DEBUG
    #define LOGGING_MODEL_VERBOSE
#endif

namespace mv {

using namespace util;
using namespace gui;

LoggingModel::Item::Item(LoggingModel& loggingModel, const MessageRecord& messageRecord) :
    QStandardItem(),
    QObject(),
    _loggingModel(loggingModel),
    _messageRecord(messageRecord)
{
    setEditable(false);
}

LoggingModel& LoggingModel::Item::getLoggingModel()
{
    return _loggingModel;
}

const LoggingModel& LoggingModel::Item::getLoggingModel() const
{
    return _loggingModel;
}

const MessageRecord& LoggingModel::Item::getMessageRecord() const
{
    return _messageRecord;
}

QVariant LoggingModel::Item::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::ForegroundRole:
        {
            switch (_messageRecord.type)
            {
                case QtDebugMsg:
                case QtWarningMsg:
                case QtInfoMsg:
                    break;

                case QtCriticalMsg:
                case QtFatalMsg:
                    return QBrush(Qt::red);
            }

            break;
        }

        default:
            break;
    }

    return {};
}

QVariant LoggingModel::NumberItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getMessageRecord().number;

        case Qt::DisplayRole:
            return QString::number(data(Qt::EditRole).toInt());

        case Qt::ToolTipRole:
            return QString("Message index: %1").arg(data(Qt::DisplayRole).toString());

        default:
            break;
    }

    return Item::data(role);
}

QVariant LoggingModel::TypeItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getMessageRecord().type;

        case Qt::DisplayRole:
            return Logger::getMessageTypeName(getMessageRecord().type);

        case Qt::ToolTipRole:
            return QString("Message type: %1").arg(data(Qt::DisplayRole).toString());

        default:
            break;
    }

    return Item::data(role);
}

LoggingModel::MessageItem::MessageItem(LoggingModel& loggingModel, const util::MessageRecord& messageRecord) :
    Item(loggingModel, messageRecord)
{
    connect(&getLoggingModel().getWordWrapAction(), &ToggleAction::toggled, this, [this](bool toggled) -> void {
        emitDataChanged();
    });
}

QVariant LoggingModel::MessageItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getMessageRecord().message;

        case Qt::DisplayRole:
        {
            if (getLoggingModel().getWordWrapAction().isChecked())
                return QVariant::fromValue(QString(getMessageRecord().message).split("\n").first());
            else
                return QVariant::fromValue(QString(getMessageRecord().message));
        }

        case Qt::ToolTipRole:
            return QString("Message: %1").arg(data(Qt::DisplayRole).toString());

        default:
            break;
    }

    return Item::data(role);
}

QVariant LoggingModel::FileAndLineItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getMessageRecord().file != nullptr ? QString("%1(%2)").arg(getMessageRecord().file).arg(QString::number(getMessageRecord().line)) : "";

        case Qt::ToolTipRole:
            return QString("File and line: %1").arg(data(Qt::DisplayRole).toString());

        default:
            break;
    }

    return Item::data(role);
}

QVariant LoggingModel::FunctionItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return QString(getMessageRecord().function);

        case Qt::ToolTipRole:
            return QString("Function: %1").arg(data(Qt::DisplayRole).toString());

        default:
            break;
    }

    return Item::data(role);
}

QVariant LoggingModel::CategoryItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return QString(getMessageRecord().category);

        case Qt::ToolTipRole:
            return QString("Category: %1").arg(data(Qt::DisplayRole).toString());

        default:
            break;
    }

    return Item::data(role);
}

LoggingModel::Row::Row(LoggingModel& loggingModel, const util::MessageRecord& messageRecord)
{
    append(new NumberItem(loggingModel, messageRecord));
    append(new TypeItem(loggingModel, messageRecord));
    append(new MessageItem(loggingModel, messageRecord));
    append(new FileAndLineItem(loggingModel, messageRecord));
    append(new FunctionItem(loggingModel, messageRecord));
    append(new CategoryItem(loggingModel, messageRecord));
}

LoggingModel::LoggingModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent),
    _messageRecords(),
    _wordWrapAction(this, "Word wrap", true),
    _idleUpdateConnection()
{
    _wordWrapAction.setToolTip("Enables/disables word wrapping");

    _idleUpdateConnection = connect(QAbstractEventDispatcher::instance(), &QAbstractEventDispatcher::awake, this, &LoggingModel::populateFromLogger);

    populateFromLogger();
}

QVariant LoggingModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    switch (static_cast<Column>(section))
    {
        case Column::Number:
            return NumberItem::headerData(orientation, role);

        case Column::Type:
            return TypeItem::headerData(orientation, role);

        case Column::Message:
            return MessageItem::headerData(orientation, role);

        case Column::FileAndLine:
            return FileAndLineItem::headerData(orientation, role);

        case Column::Function:
            return FunctionItem::headerData(orientation, role);

        case Column::Category:
            return CategoryItem::headerData(orientation, role);

        default:
            break;
    }

    return {};
}

void LoggingModel::populateFromLogger()
{
    auto& logger = Application::current()->getLogger();

    const auto previousNumberOfMessages = _messageRecords.size();
    const auto numberOfAddedMessages    = logger.getMessageRecords().size() - previousNumberOfMessages;

    if (numberOfAddedMessages >= 1) {
        logger.updateMessageRecords(_messageRecords);

        for (size_t recordIndex = previousNumberOfMessages; recordIndex < (previousNumberOfMessages + numberOfAddedMessages); ++recordIndex)
            appendRow(Row(*this, *_messageRecords[recordIndex]));
    }
}

}