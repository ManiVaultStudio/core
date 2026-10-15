// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "pointdata_export.h"

#include <QAbstractTableModel>

#include <cstdint>
#include <vector>

class Points;

/**
 * @brief Table model that exposes a fetched point-data window to Qt views.
 *
 * The model deliberately separates the logical table dimensions from the
 * currently loaded window. Qt can therefore represent very large point
 * datasets without requiring a value for every cell in memory. The view asks
 * the model to fetch the visible range after navigation, and the model keeps
 * only that range for display.
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT PointsTableModel final : public QAbstractTableModel
{
    Q_OBJECT

public:
    /** Constructs an empty point-data table model. */
    explicit PointsTableModel(QObject* parent = nullptr);

    /** Selects the point data and logical table dimensions displayed by the model. */
    void setPoints(Points* points, std::uint64_t rows, std::uint64_t columns);

    /** Clears the selected data and any fetched values. */
    void clear();

    /** Returns whether a point-data source is selected. */
    bool hasPointData() const
    {
        return _points != nullptr;
    }

    /** Returns the total number of rows in the selected data. */
    std::uint64_t totalRows() const
    {
        return _totalRows;
    }

    /** Returns the total number of columns in the selected data. */
    std::uint64_t totalColumns() const
    {
        return _totalColumns;
    }

    /** Fetches a rectangular window and emits a data-change notification for it. */
    std::size_t fetchRange(std::uint64_t firstRow, std::uint64_t rowCount, std::uint64_t firstColumn, std::uint64_t columnCount);

    /** Replaces the visible table with an explicitly fetched query result. */
    void setQueryResult(const std::vector<std::uint64_t>& rowIds, const std::vector<std::uint64_t>& columnIds, const std::vector<float>& values);

    /** Returns whether the model currently displays an explicit query result. */
    bool isQueryResult() const
    {
        return _queryResult;
    }

    /** Returns the most recent tile-access error, if any. */
    QString getLastError() const
    {
        return _lastError;
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    void notifyRange(std::uint64_t firstRow, std::uint64_t rowCount, std::uint64_t firstColumn, std::uint64_t columnCount);

    Points*                    _points            = nullptr;
    std::uint64_t              _totalRows         = 0;
    std::uint64_t              _totalColumns      = 0;
    std::uint64_t              _loadedFirstRow    = 0;
    std::uint64_t              _loadedRowCount    = 0;
    std::uint64_t              _loadedFirstColumn = 0;
    std::uint64_t              _loadedColumnCount = 0;
    std::vector<float>         _values;
    bool                       _queryResult       = false;
    std::vector<std::uint64_t> _queryRowIds;
    std::vector<std::uint64_t> _queryColumnIds;
    QString                    _lastError;
};
