// Its own header file:
#include "LogItemModel.h"

#include "Logger.h"

// Qt header files:
#include <QBrush>
#include <QDateTime>
#include <QFileInfo>
#include <QRegExp>
#include <QStringList>

// Standard C++ header files:
#include <algorithm>
#include <cassert>
#include <cstdint> // For uintmax_t
#include <tuple>


namespace
{

QString MsgTypeToString(const QtMsgType& msgType)
{
    switch (msgType)
    {
    case QtDebugMsg: return "debug";
    case QtWarningMsg: return "warning";
    case QtCriticalMsg: return "critical";
    case QtFatalMsg: return "fatal";
    case QtInfoMsg: return "info";
    }
    return QString{}.setNum(msgType);
}

}   // namespace

namespace hdps
{
namespace gui
{

LogItemModel::LogItemModel()
{
    Logger::GetMessageRecords(_messageRecords);
}


// Returns true when new messages were added.
bool LogItemModel::Reload()
{
    const auto numberOfMessages = _messageRecords.size();

    Logger::GetMessageRecords(_messageRecords);

    if (_messageRecords.size() > numberOfMessages)
    {
        if (_sortedMessageRecords.size() != _messageRecords.size())
        {
            // Clear the sorted records, to trigger redoing the sort.
            _sortedMessageRecords.clear();
        }
        return true;
    }
    else
    {
        assert(_messageRecords.size() == numberOfMessages);
        assert(_sortedMessageRecords.empty() ||
            (_sortedMessageRecords.size() == numberOfMessages));

        // There were no new messages.
        return false;
    }
}



const MessageRecord* LogItemModel::GetMessageRecordAtRow(const unsigned row) const
{
    const auto numberOfMessages = _messageRecords.size();

    if (row < numberOfMessages)
    {
        const auto index = _isSortedInDescendingOrder ?
            (numberOfMessages - static_cast<unsigned>(row) - 1) : static_cast<unsigned>(row);

        const auto& messageRecords = (_sortedMessageRecords.empty() ?
            _messageRecords : _sortedMessageRecords);
        return messageRecords[index];
    }
    else
    {
        return nullptr;
    }
}


QVariant LogItemModel::GetDataValueAtColumn(const MessageRecord& messageRecord, const unsigned column)
{
    if (column < numberOfColumns)
    {
        switch (static_cast<ColumnEnum>(column))
        {
        case ColumnEnum::number:
        {
            return messageRecord.number;
        }
        case ColumnEnum::category:
        {
            return messageRecord.category;
        }
        case ColumnEnum::type:
        {
            return MsgTypeToString(messageRecord.type);
        }
        case ColumnEnum::fileAndLine:
        {
            return (messageRecord.file == nullptr) && (messageRecord.line == 0) ?
                QString{} :
                (QString("%1(%2)")
                    .arg(messageRecord.file)
                    .arg(messageRecord.line));
        }
        case ColumnEnum::function:
        {
            return messageRecord.function;
        }
        case ColumnEnum::message:
        {
            return messageRecord.message;
        }
        }
    }
    return {};
}


LogItemModel::~LogItemModel() = default;


int LogItemModel::rowCount(const QModelIndex &) const
{
    return static_cast<int>(_messageRecords.size());
}


int LogItemModel::columnCount(const QModelIndex &) const
{
    return int{ numberOfColumns };
}


QModelIndex LogItemModel::index(const int row, const int column, const QModelIndex &) const
{
    return createIndex(row, column);
}


QModelIndex LogItemModel::parent(const QModelIndex &) const
{
    return{};
}


QVariant LogItemModel::data(const QModelIndex & modelIndex, const int role) const
{
    try
    {
        if (role == Qt::DisplayRole)
        {
            const auto row = modelIndex.row();
            const auto column = modelIndex.column();

            const auto numberOfMessages = _messageRecords.size();

            if ((row >= 0) && (column >= 0) )
            {
                const auto* const messageRecord = GetMessageRecordAtRow(static_cast<unsigned>(row));

                if (messageRecord != nullptr)
                {
                    return GetDataValueAtColumn(*messageRecord, static_cast<unsigned>(column));
                }
            }
        }
    }
    catch (const std::exception&)
    {
        // Out of luck.
    }
    return QVariant{};
}


QVariant LogItemModel::headerData(const int section, const Qt::Orientation orientation, const int role) const
{
    try
    {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        {
            const char* const headers[] =
            {
                "Number",
                "Category",
                "Type",
                "Source file (line of code)",
                "Function",
                "Message"
            };

            static_assert(
                sizeof(headers) / sizeof(headers[0]) == numberOfColumns,
                "Each column should have a header!");

            return QString::fromLatin1(headers[section]);
        }
    }
    catch (const std::exception&)
    {
        assert(!"Exceptions are not allowed here!");
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}


namespace
{
    template <typename T>
    void Sort(std::deque<const MessageRecord*>& messageRecords, const T compare)
    {
        std::sort(messageRecords.begin(), messageRecords.end(),
            [compare](const MessageRecord* const lhs, const MessageRecord* const rhs)
        {
            assert(lhs != nullptr);
            assert(rhs != nullptr);
            const auto comparison = compare(*lhs, *rhs);
            return (comparison < 0) || ((comparison == 0) && (lhs->number < rhs->number));
        }
        );
    }

    int CompareIntegers(const int lhs, const int rhs)
    {
        return (lhs < rhs) ? -1 : (rhs < lhs) ? 1 : 0;
    }

    int CompareStrings(const char* const lhs, const char* const rhs)
    {
        return (lhs == rhs) ? 0 : 
            (lhs == nullptr) ? -1 :
            (rhs == nullptr) ? 1 :
            std::strcmp(lhs, rhs);
    }
}


void LogItemModel::SortMessageRecords(const ColumnEnum column)
{
    switch (static_cast<ColumnEnum>(column))
    {
    case ColumnEnum::category:
    {
        return Sort(_sortedMessageRecords,
            [](const MessageRecord& lhs, const MessageRecord& rhs)
        {
            return CompareStrings(lhs.category, rhs.category);
        });
    }
    case ColumnEnum::type:
    {
        return Sort(_sortedMessageRecords,
            [](const MessageRecord& lhs, const MessageRecord& rhs)
        {
            return CompareIntegers(lhs.type, rhs.type);
        });
    }
    case ColumnEnum::fileAndLine:
    {
        return Sort(_sortedMessageRecords,
            [](const MessageRecord& lhs, const MessageRecord& rhs)
        {
            const auto comparison = CompareStrings(lhs.file, rhs.file);
            return (comparison == 0) ? CompareIntegers(lhs.line, rhs.line) : comparison;
        });
    }
    case ColumnEnum::function:
    {
        return Sort(_sortedMessageRecords,
            [](const MessageRecord& lhs, const MessageRecord& rhs)
        {
            return CompareStrings(lhs.function, rhs.function);
        });
    }
    case ColumnEnum::message:
    {
        return Sort(_sortedMessageRecords,
            [](const MessageRecord& lhs, const MessageRecord& rhs)
        {
            return lhs.message.compare(rhs.message);
        });
    }
    }
    assert(!"Unhandled column!");
}


void LogItemModel::sort(const int column, const Qt::SortOrder order)
{
    if ((column >= 0) && (static_cast<std::uintmax_t>(column) < numberOfColumns))
    {
        const auto columnEnum = static_cast<ColumnEnum>(column);

        _isSortedInDescendingOrder = (order == Qt::DescendingOrder);
        assert(_isSortedInDescendingOrder || (order == Qt::AscendingOrder));

        beginResetModel();

        struct ResetModelGuard
        {
            QAbstractItemModel& itemModel;
            void (QAbstractItemModel::* const endResetModelPtr)();

            ~ResetModelGuard()
            {
                (itemModel.*endResetModelPtr)();
            }
        };

        const ResetModelGuard resetModelGuard{ *this, &LogItemModel::endResetModel };

        if (columnEnum == ColumnEnum::number)
        {
            // No need to sort. Just use the order of the messages as they have been entered.
            _sortedMessageRecords.clear();
            _sortedMessageRecords.shrink_to_fit();
        }
        else
        {
            if (_sortedMessageRecords.size() == _messageRecords.size())
            {
                // All messages have been sorted before.
                if (_sortedColumn != column)
                {
                    // All messages have been sorted before, but on a different column.
                    // Sort again!
                    SortMessageRecords(columnEnum);
                }
            }
            else
            {
                assert(_sortedMessageRecords.size() < _messageRecords.size());

                // m_sortedMessageRecords does not (yet) have a record for each message,
                // so copy all of them, and then sort.
                _sortedMessageRecords = _messageRecords;
                SortMessageRecords(columnEnum);
            }
        }
        _sortedColumn = column;
    }
    else
    {
        assert(column == -1);
    }
}

}   // namespace gui
}   // namespace hdps
