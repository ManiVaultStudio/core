// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QAbstractTableModel>

#include <cstdint>
#include <vector>

class PointData;

class PointsTableModel final : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit PointsTableModel(QObject* parent = nullptr);

    void setPointData(PointData* pointData, std::uint64_t rows, std::uint64_t columns);
    void clear();
    bool hasPointData() const
    {
        return _pointData != nullptr;
    }

    std::uint64_t totalRows() const
    {
        return _totalRows;
    }

    std::uint64_t totalColumns() const
    {
        return _totalColumns;
    }

    std::size_t fetchRange(std::uint64_t firstRow, std::uint64_t rowCount, std::uint64_t firstColumn, std::uint64_t columnCount);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    void notifyRange(std::uint64_t firstRow, std::uint64_t rowCount, std::uint64_t firstColumn, std::uint64_t columnCount);

    PointData* _pointData = nullptr;
    std::uint64_t _totalRows = 0;
    std::uint64_t _totalColumns = 0;
    std::uint64_t _loadedFirstRow = 0;
    std::uint64_t _loadedRowCount = 0;
    std::uint64_t _loadedFirstColumn = 0;
    std::uint64_t _loadedColumnCount = 0;
    std::vector<float> _values;
};
