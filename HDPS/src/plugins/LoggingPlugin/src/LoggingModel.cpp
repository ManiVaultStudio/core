#include "LoggingModel.h"

#include <Application.h>

#include <util/Logger.h>

#include <QBrush>
#include <QDateTime>
#include <QFileInfo>
#include <QRegularExpression>
#include <QStringList>

#include <algorithm>
#include <cassert>

#ifdef _DEBUG
    #define LOGGING_MODEL_VERBOSE
#endif

using namespace hdps;
using namespace hdps::util;

QMap<LoggingModel::Column, QString> LoggingModel::columnNames = {
    { Column::Number, "Number" },
    { Column::Category, "Category" },
    { Column::Type, "Type" },
    { Column::FileAndLine, "File and line" },
    { Column::Function, "Function" },
    { Column::Message, "Message" },
    { Column::Count, "Number of columns" }
};

LoggingModel::LoggingModel(QObject* parent /*= nullptr*/) :
    QAbstractItemModel(parent)
{
    synchronizeLogRecords();
}

int LoggingModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
        return static_cast<int>(_messageRecords.size());

    return 0;
}

int LoggingModel::columnCount(const QModelIndex& parent) const
{
    return static_cast<int>(Column::Count);
}

QModelIndex LoggingModel::index(const int row, const int column, const QModelIndex& parent) const
{
    if (row < 0 || row >= rowCount(parent))
        return QModelIndex();

    if (column < 0 || column >= columnCount(parent))
        return QModelIndex();

    return createIndex(row, column, _messageRecords[row]);
}

QVariant LoggingModel::data(const QModelIndex& index, const int role) const
{
    const auto* messageRecord = static_cast<MessageRecord*>(index.internalPointer());

    switch (role) {
        case Qt::DisplayRole:
        {
            switch (static_cast<Column>(index.column()))
            {
                case Column::Number:
                    return QVariant::fromValue(QString::number(data(index, Qt::EditRole).toULongLong()));

                case Column::Type:
                case Column::Message:
                case Column::FileAndLine:
                case Column::Function:
                case Column::Category:
                    return QVariant::fromValue(data(index, Qt::EditRole).toString());
            }

            break;
        }

        case Qt::EditRole:
        {
            switch (static_cast<Column>(index.column()))
            {
                case Column::Number:
                    return QVariant::fromValue(qulonglong{ messageRecord->number });
                
                case Column::Type:
                    return QVariant::fromValue(Logger::getMessageTypeName(messageRecord->type));

                case Column::Message:
                    return QVariant::fromValue(QString(messageRecord->message));

                case Column::FileAndLine:
                {
                    if (messageRecord->file == nullptr)
                        break;

                    return QVariant::fromValue(QString("%1(%2)").arg(messageRecord->file).arg(messageRecord->line));
                }

                case Column::Function:
                    return QVariant::fromValue(QString(messageRecord->function));

                case Column::Category:
                    return QVariant::fromValue(QString(messageRecord->category));
            }

            break;
        }

        case Qt::ToolTipRole:
            return QVariant::fromValue(data(index, Qt::DisplayRole).toString());

        case Qt::TextAlignmentRole:
        {
            if (index.column() == static_cast<std::int32_t>(Column::Number))
                return Qt::AlignLeft;

            break;
        }
    }

    return QVariant();
}


QVariant LoggingModel::headerData(const int section, const Qt::Orientation orientation, const int role) const
{
    if (orientation == Qt::Horizontal) {
        switch (role) {
            case Qt::DisplayRole:
            {
                if (section == static_cast<std::int32_t>(Column::Number))
                    return "#";

                if (section < static_cast<std::int32_t>(Column::Count))
                    return columnNames[static_cast<Column>(section)];

                break;
            }

            case Qt::EditRole:
            {
                if (section < static_cast<std::int32_t>(Column::Count))
                    return columnNames[static_cast<Column>(section)];

                break;
            }

            case Qt::ToolTipRole:
            {
                if (section < static_cast<std::int32_t>(Column::Count))
                    return columnNames[static_cast<Column>(section)];

                break;
            }
        }
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

void LoggingModel::synchronizeLogRecords()
{
    auto& logger = Application::current()->getLogger();

    const auto previousNumberOfMessages = _messageRecords.size();
    const auto numberOfAddedMessages    = logger.getMessageRecords().size() - previousNumberOfMessages;

    if (numberOfAddedMessages >= 1) {
        const auto startRowIndex = rowCount(QModelIndex());

        beginInsertRows(QModelIndex(), startRowIndex, startRowIndex + static_cast<int>(numberOfAddedMessages) - 1);
        {
            logger.updateMessageRecords(_messageRecords);
        }
        endInsertRows();
    }
}

QModelIndex LoggingModel::parent(const QModelIndex& index) const
{
    return QModelIndex();
}

Qt::ItemFlags LoggingModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    auto itemFlags = Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);

    return itemFlags;
}
