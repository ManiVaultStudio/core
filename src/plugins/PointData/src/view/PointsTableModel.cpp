// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PointsTableModel.h"

#include "../PointData.h"

#include <algorithm>
#include <exception>
#include <limits>
#include <utility>

namespace
{
float valueAt(const PointDataAccessView& view, const std::uint64_t row, const std::uint64_t column)
{
    const auto* bytes = view.rowData(row) + column * view.elementSizeBytes;

    switch (view.elementTypeIndex)
    {
        case 0:
            return *reinterpret_cast<const float*>(bytes);

        case 1:
            return static_cast<float>(*reinterpret_cast<const biovault::bfloat16_t*>(bytes));

        case 2:
            return static_cast<float>(*reinterpret_cast<const std::int32_t*>(bytes));

        case 3:
            return static_cast<float>(*reinterpret_cast<const std::uint32_t*>(bytes));

        case 4:
            return static_cast<float>(*reinterpret_cast<const std::int16_t*>(bytes));

        case 5:
            return static_cast<float>(*reinterpret_cast<const std::uint16_t*>(bytes));

        case 6:
            return static_cast<float>(*reinterpret_cast<const std::int8_t*>(bytes));

        case 7:
            return static_cast<float>(*reinterpret_cast<const std::uint8_t*>(bytes));

        default:
            return 0.0f;
    }
}

int toModelIndex(const std::uint64_t value)
{
    return value > static_cast<std::uint64_t>(std::numeric_limits<int>::max())
        ? std::numeric_limits<int>::max()
        : static_cast<int>(value);
}
}

PointsTableModel::PointsTableModel(QObject* parent) :
    QAbstractTableModel(parent)
{
}

void PointsTableModel::setPoints(Points* points, const std::uint64_t rows, const std::uint64_t columns)
{
    beginResetModel();

    _points          = points;
    _totalRows       = rows;
    _totalColumns    = columns;
    _windowFirstRow  = 0;
    _windowRowCount  = std::min<std::uint64_t>(rows, 64);
    _dimensionNames = points ? points->getDimensionNames() : std::vector<QString>();
    _loadedFirstRow = _loadedRowCount = _loadedFirstColumn = _loadedColumnCount = 0;

    _values.clear();
    _queryResult = false;
    _queryRowIds.clear();
    _queryColumnIds.clear();
    _lastError.clear();

    endResetModel();
}

void PointsTableModel::setWindow(const std::uint64_t firstRow, const std::uint64_t rowCount)
{
    const auto availableRows = _queryResult ? static_cast<std::uint64_t>(_queryRowIds.size()) : _totalRows;
    const auto boundedFirstRow = std::min(firstRow, availableRows);
    const auto boundedRowCount = std::min(rowCount, availableRows - boundedFirstRow);
    if (_windowFirstRow == boundedFirstRow && _windowRowCount == boundedRowCount)
        return;

    const auto rowCountChanged = _windowRowCount != boundedRowCount;
    if (rowCountChanged)
        beginResetModel();

    const auto oldLoadedFirstColumn = _loadedFirstColumn;
    const auto oldLoadedColumnCount = _loadedColumnCount;
    _windowFirstRow = boundedFirstRow;
    _windowRowCount = boundedRowCount;
    if (!_queryResult)
    {
        _loadedFirstRow = _loadedRowCount = 0;
        _loadedFirstColumn = _loadedColumnCount = 0;
        _values.clear();
        _lastError.clear();
    }

    if (rowCountChanged)
    {
        endResetModel();
    }
    else
    {
        if (!_queryResult && oldLoadedColumnCount > 0)
            notifyRange(0, _windowRowCount, oldLoadedFirstColumn, oldLoadedColumnCount);
        else if (_queryResult && _windowRowCount > 0 && _totalColumns > 0)
            notifyRange(0, _windowRowCount, 0, _queryColumnIds.size());

        if (_windowRowCount > 0)
            emit headerDataChanged(Qt::Vertical, 0, toModelIndex(_windowRowCount - 1));
    }
}

void PointsTableModel::clear()
{
    setPoints(nullptr, 0, 0);
}

int PointsTableModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : toModelIndex(_windowRowCount);
}

int PointsTableModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : toModelIndex(_queryResult ? _queryColumnIds.size() : _totalColumns);
}

QVariant PointsTableModel::data(const QModelIndex& index, const int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole || !_points)
        return {};

    const auto row    = _windowFirstRow + static_cast<std::uint64_t>(index.row());
    const auto column = static_cast<std::uint64_t>(index.column());

    if (_queryResult)
    {
        const auto queryRow = _windowFirstRow + static_cast<std::uint64_t>(index.row());
        return QString::number(_values[static_cast<std::size_t>(queryRow * _queryColumnIds.size() + column)], 'g', 7);
    }

    if (row < _loadedFirstRow || row >= _loadedFirstRow + _loadedRowCount
        || column < _loadedFirstColumn || column >= _loadedFirstColumn + _loadedColumnCount)
        return QStringLiteral("…");

    const auto value = _values[static_cast<std::size_t>((row - _loadedFirstRow) * _loadedColumnCount + column - _loadedFirstColumn)];

    return QString::number(value, 'g', 7);
}

QVariant PointsTableModel::headerData(const int section, const Qt::Orientation orientation, const int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (_queryResult)
    {
        const auto ids = orientation == Qt::Horizontal ? &_queryColumnIds : &_queryRowIds;

        if (section >= static_cast<int>(ids->size()))
            return {};

        const auto id = ids->at(static_cast<std::size_t>(orientation == Qt::Horizontal
            ? static_cast<std::uint64_t>(section)
            : _windowFirstRow + static_cast<std::uint64_t>(section)));

        if (orientation == Qt::Horizontal && id < _dimensionNames.size() && !_dimensionNames[id].isEmpty())
            return _dimensionNames[id];

        return QString::number(id);
    }

    if (orientation == Qt::Horizontal && section < static_cast<int>(_dimensionNames.size()) && !_dimensionNames[static_cast<std::size_t>(section)].isEmpty())
        return _dimensionNames[static_cast<std::size_t>(section)];

    return orientation == Qt::Horizontal ? QVariant(QString("D%1").arg(section)) : QVariant(QString::number(_windowFirstRow + static_cast<std::uint64_t>(section)));
}

std::size_t PointsTableModel::fetchRange(const std::uint64_t firstRow, const std::uint64_t rowCount, const std::uint64_t firstColumn, const std::uint64_t columnCount)
{
    setWindow(firstRow, rowCount);
    return applyRangeResult(loadRange(_points, _totalRows, _totalColumns, firstRow, rowCount, firstColumn, columnCount));
}

PointsTableModel::RangeResult PointsTableModel::loadRange(Points* points, const std::uint64_t totalRows, const std::uint64_t totalColumns, const std::uint64_t firstRow, const std::uint64_t rowCount, const std::uint64_t firstColumn, const std::uint64_t columnCount, const std::uint64_t generation)
{
    RangeResult result;
    result.generation = generation;

    if (!points || firstRow >= totalRows || firstColumn >= totalColumns || rowCount == 0 || columnCount == 0)
        return result;

    const auto boundedRowCount    = std::min(rowCount, totalRows - firstRow);
    const auto boundedColumnCount = std::min(columnCount, totalColumns - firstColumn);
    result.firstRow = firstRow;
    result.rowCount = boundedRowCount;
    result.firstColumn = firstColumn;
    result.columnCount = boundedColumnCount;
    result.values.assign(static_cast<std::size_t>(boundedRowCount * boundedColumnCount), 0.0f);

    std::uint64_t destinationRow    = 0;
    std::uint64_t destinationColumn = 0;

    try
    {
        points->visitRange(firstRow, boundedRowCount, firstColumn, boundedColumnCount, [&result, &destinationRow, &destinationColumn](const PointDataAccessView& view) {
            ++result.accessSegmentCount;

            for (std::uint64_t row = 0; row < view.rows; ++row)
            {
                for (std::uint64_t column = 0; column < view.columns; ++column)
                {
                    const auto targetRow    = destinationRow + row;
                    const auto targetColumn = destinationColumn + column;

                    if (targetRow < result.rowCount && targetColumn < result.columnCount)
                    {
                        result.values[static_cast<std::size_t>(targetRow * result.columnCount + targetColumn)] = valueAt(view, row, column);
                    }
                }
            }

            destinationColumn += view.columns;

            if (destinationColumn >= result.columnCount)
            {
                destinationColumn = 0;
                destinationRow += view.rows;
            }
        });
    }
    catch (const std::exception& exception)
    {
        result.error = QString::fromUtf8(exception.what());
    }
    catch (...)
    {
        result.error = QStringLiteral("Unknown error while loading point-data tiles");
    }

    return result;
}

std::size_t PointsTableModel::applyRangeResult(RangeResult result)
{
    if (result.rowCount == 0 || result.columnCount == 0 || result.firstRow < _windowFirstRow)
        return 0;

    const auto relativeFirstRow = result.firstRow - _windowFirstRow;
    if (relativeFirstRow >= _windowRowCount || result.rowCount > _windowRowCount - relativeFirstRow)
        return 0;

    const auto oldFirstColumn = _loadedFirstColumn;
    const auto oldColumnCount = _loadedColumnCount;
    _lastError = result.error;

    if (!_lastError.isEmpty())
    {
        _values.clear();
        _loadedRowCount = _loadedColumnCount = 0;
        notifyRange(0, _windowRowCount, oldFirstColumn, oldColumnCount);
        return 0;
    }
    _loadedFirstRow = result.firstRow;
    _loadedRowCount = result.rowCount;
    _loadedFirstColumn = result.firstColumn;
    _loadedColumnCount = result.columnCount;
    _values = std::move(result.values);

    notifyRange(0, _windowRowCount, oldFirstColumn, oldColumnCount);
    notifyRange(0, _windowRowCount, _loadedFirstColumn, _loadedColumnCount);

    return result.accessSegmentCount;
}

void PointsTableModel::setQueryResult(const std::vector<std::uint64_t>& rowIds, const std::vector<std::uint64_t>& columnIds, const std::vector<float>& values)
{
    beginResetModel();

    _queryResult    = true;
    _queryRowIds    = rowIds;
    _queryColumnIds = columnIds;
    _values         = values;
    _windowFirstRow = 0;
    _windowRowCount = std::min<std::uint64_t>(static_cast<std::uint64_t>(_queryRowIds.size()), 64);

    _lastError.clear();

    _loadedFirstRow    = 0;
    _loadedRowCount    = 0;
    _loadedFirstColumn = 0;
    _loadedColumnCount = 0;

    endResetModel();
}

void PointsTableModel::notifyRange(const std::uint64_t firstRow, const std::uint64_t rowCount, const std::uint64_t firstColumn, const std::uint64_t columnCount)
{
    if (rowCount == 0 || columnCount == 0)
        return;

    emit dataChanged(index(toModelIndex(firstRow), toModelIndex(firstColumn)), index(toModelIndex(firstRow + rowCount - 1), toModelIndex(firstColumn + columnCount - 1)), { Qt::DisplayRole });
}
