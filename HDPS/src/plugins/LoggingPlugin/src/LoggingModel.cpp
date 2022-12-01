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

QMap<LoggingModel::Column, QString> LoggingModel::columnNames = {
    { Column::Number, "Number" },
    { Column::Category, "Category" },
    { Column::Type, "Type" },
    { Column::FileAndLine, "FileAndLine" },
    { Column::Function, "Function" },
    { Column::Message, "Message" },
    { Column::Count, "NumberOfColumns" }
};

LoggingModel::LoggingModel(QObject* parent /*= nullptr*/) :
    QAbstractItemModel(parent)
{
    synchronizeLogRecords();
}

LoggingModel::~LoggingModel() = default;

int LoggingModel::rowCount(const QModelIndex& modelIndex) const
{
    if (!modelIndex.isValid())
        return static_cast<int>(_messageRecords.size());

    return 0;
}

int LoggingModel::columnCount(const QModelIndex &) const
{
    return static_cast<int>(Column::Count);
}

QModelIndex LoggingModel::index(const int row, const int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid())
        return QModelIndex();

    return createIndex(row, column, static_cast<const void*>(_messageRecords.at(row)));
}

QVariant LoggingModel::data(const QModelIndex& modelIndex, const int role) const
{
    const auto* messageRecord = static_cast<hdps::MessageRecord*>(modelIndex.internalPointer());

    switch (role) {
        case Qt::DisplayRole:
        {
            switch (static_cast<Column>(modelIndex.column()))
            {
                case Column::Number:
                    return QVariant::fromValue(QString::number(data(modelIndex, Qt::EditRole).toULongLong()));
            }

            return "===";
            break;
        }

        case Qt::EditRole:
        {
            switch (static_cast<Column>(modelIndex.column()))
            {
                case Column::Number:
                    return qulonglong{ messageRecord->number };

                
                case Column::Category:
                    return messageRecord->category;
                
                    /*
                case Column::Type:
                {
                    return hdps::Logger::MsgTypeToString(messageRecord.type);
                }

                case Column::FileAndLine:
                {
                    return (messageRecord.file == nullptr) && (messageRecord.line == 0) ?
                        QString{} :
                        (QString("%1(%2)")
                            .arg(messageRecord.file)
                            .arg(messageRecord.line));
                }

                case Column::function:
                {
                    return messageRecord.function;
                }

                case Column::Message:
                {
                    return messageRecord.message;
                }*/
            }

            break;
        }
    }

    return "TEST";

    return QVariant{};
}


QVariant LoggingModel::headerData(const int section, const Qt::Orientation orientation, const int role) const
{
    if (orientation == Qt::Horizontal) {
        switch (role) {
            case Qt::DisplayRole:
            case Qt::EditRole:
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
#ifdef LOGGING_MODEL_VERBOSE
    //qDebug() << __FUNCTION__;
#endif

    const auto previousNumberOfMessages = _messageRecords.size();

    Application::current()->getLogger().updateMessageRecords(_messageRecords);

    const auto numberOfAddedMessages = _messageRecords.size() - previousNumberOfMessages;

    if (numberOfAddedMessages >= 1) {
        //qDebug() << numberOfMessages << numberOfAddedMessages;

        //layoutAboutToBeChanged();
        beginInsertRows(QModelIndex(), 0, numberOfAddedMessages - 1);
        {
        }
        endInsertRows();

        //emit layoutChanged();

        //emit dataChanged(QModelIndex(), QModelIndex());
        //beginLayout
    }

    //Application::processEvents();
}

QModelIndex LoggingModel::parent(const QModelIndex& index) const
{
    return QModelIndex();
    //if (!index.isValid())
        

    return createIndex(index.row(), 0);
}
