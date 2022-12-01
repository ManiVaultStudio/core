#pragma once

#include <util/Logger.h>

#include <QAbstractItemModel>

#include <deque>
#include <cstdint>

struct MessageRecord;

class LoggingModel final : public QAbstractItemModel
{
public:
    enum class Column
    {
        Number,
        Type,
        Message,
        FileAndLine,
        Function,
        Category,
        Count
    };

    static QMap<Column, QString> columnNames;

    

public:
    LoggingModel(QObject* parent = nullptr);
    LoggingModel(const LoggingModel&) = delete;
    ~LoggingModel() override;

    LoggingModel& operator=(const LoggingModel&) = delete;

    /** Synchronizes the model with the log records from the core logger */
    void synchronizeLogRecords();

private:
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QModelIndex index(int, int, const QModelIndex &) const  override;

    /**
     * Mandatory override for QAbstractItemModel. Returns the index of the parent
     * of this item. If this item is not a child, an invalid index is returned.
     */
    QModelIndex parent(const QModelIndex& index) const override;

    QVariant data(const QModelIndex &, int) const override;
    QVariant headerData(int, Qt::Orientation, int) const override;

    /**
     * Get model item flags
     * @param index Model index
     * @return Model item flags
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    std::deque<const hdps::MessageRecord*> _messageRecords;
};
