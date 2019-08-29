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

enum class ColumnEnum
{
    category,
    type,
    fileAndLine,
    function,
    message,
    numberOfColumns
};

constexpr auto numberOfColumns = static_cast<std::uintmax_t>(ColumnEnum::numberOfColumns);


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
    Logger::GetMessageRecords(m_deque);
}


void LogItemModel::Reload()
{
    Logger::GetMessageRecords(m_deque);
}


std::uint16_t LogItemModel::GetNumberOfColumns()
{
    return std::uint16_t{ numberOfColumns };
}


LogItemModel::~LogItemModel() = default;


int LogItemModel::rowCount(const QModelIndex &) const
{
    return static_cast<int>(m_deque.size());
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

            if ((row >= 0) && (static_cast<std::uintmax_t>(row) < m_deque.size())
                && (column >= 0) && (static_cast<std::uintmax_t>(column) < numberOfColumns))
            {
                const auto* const messageRecord = m_deque[static_cast<unsigned>(row)];

                if (messageRecord != nullptr)
                {
                    switch (static_cast<ColumnEnum>(column))
                    {
                    case ColumnEnum::category:
                    {
                        return messageRecord->category;
                    }
                    case ColumnEnum::type:
                    {
                        return MsgTypeToString(messageRecord->type);
                    }
                    case ColumnEnum::fileAndLine:
                    {
                        return QString("%1(%2)")
                            .arg(messageRecord->file)
                            .arg(messageRecord->line);
                    }
                    case ColumnEnum::function:
                    {
                        return messageRecord->function;
                    }
                    case ColumnEnum::message:
                    {
                        return messageRecord->message;
                    }
                    }
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
            "Category",
            "Type",
            "Source file (line of code)",
            "Function",
            "Message"
            };
            return QString::fromLatin1(headers[section]);
        }
    }
    catch (const std::exception&)
    {
        assert(!"Exceptions are not allowed here!");
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

}   // namespace gui
}   // namespace hdps
