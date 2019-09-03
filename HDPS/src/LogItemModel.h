#pragma once

// Qt header file:
#include <QAbstractItemModel>

// Standard C++ header files:
#include <deque>
#include <cstdint> // For uint16_t

namespace hdps
{
struct MessageRecord;

namespace gui
{

class LogItemModel final : public QAbstractItemModel
{
private: // Data member:
    std::deque<const MessageRecord*> _messageRecords;
    std::deque<const MessageRecord*> _sortedMessageRecords;
    bool _isSortedInDescendingOrder{ false };
    int _sortedColumn{ -1 };

public:
    LogItemModel();
    ~LogItemModel() override;

    LogItemModel(const LogItemModel&) = delete;
    LogItemModel& operator=(const LogItemModel&) = delete;

    void Reload();
    static std::uint16_t GetNumberOfColumns();

private: // Member functions:

    // Implementation of pure virtual functions of QAbstractItemModel
    int rowCount(const QModelIndex &) const override;
    int columnCount(const QModelIndex &) const override;
    QModelIndex index(int, int, const QModelIndex &) const  override;
    QModelIndex parent(const QModelIndex &) const override;
    QVariant data(const QModelIndex &, int) const override;
    QVariant headerData(int, Qt::Orientation, int) const override;

    // Overrides QAbstractItemModel.
    void sort(int column, Qt::SortOrder order) override;
};

}   //namespace gui
}   // namespace hdps
