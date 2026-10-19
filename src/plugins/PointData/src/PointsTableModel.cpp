// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PointsTableModel.h"

#include "PointData.h"

#include <algorithm>
#include <exception>
#include <limits>

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
    _dimensionNames = points ? points->getDimensionNames() : std::vector<QString>();
    _loadedFirstRow = _loadedRowCount = _loadedFirstColumn = _loadedColumnCount = 0;

    _values.clear();
    _queryResult = false;
    _queryRowIds.clear();
    _queryColumnIds.clear();
    _lastError.clear();

    endResetModel();
}

void PointsTableModel::clear()
{
    setPoints(nullptr, 0, 0);
}

int PointsTableModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : toModelIndex(_queryResult ? _queryRowIds.size() : _totalRows);
}

int PointsTableModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : toModelIndex(_queryResult ? _queryColumnIds.size() : _totalColumns);
}

QVariant PointsTableModel::data(const QModelIndex& index, const int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole || !_points)
        return {};

    const auto row    = static_cast<std::uint64_t>(index.row());
    const auto column = static_cast<std::uint64_t>(index.column());

    if (_queryResult)
        return QString::number(_values[static_cast<std::size_t>(row * _queryColumnIds.size() + column)], 'g', 7);

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

        const auto id = ids->at(static_cast<std::size_t>(section));

        if (orientation == Qt::Horizontal && id < _dimensionNames.size() && !_dimensionNames[id].isEmpty())
            return _dimensionNames[id];

        return QString::number(id);
    }

    if (orientation == Qt::Horizontal && section < static_cast<int>(_dimensionNames.size()) && !_dimensionNames[static_cast<std::size_t>(section)].isEmpty())
        return _dimensionNames[static_cast<std::size_t>(section)];

    return orientation == Qt::Horizontal ? QVariant(QString("D%1").arg(section)) : QVariant(QString::number(section));
}

std::size_t PointsTableModel::fetchRange(const std::uint64_t firstRow, const std::uint64_t rowCount, const std::uint64_t firstColumn, const std::uint64_t columnCount)
{
    _lastError.clear();

    if (!_points || firstRow >= _totalRows || firstColumn >= _totalColumns || rowCount == 0 || columnCount == 0)
        return 0;

    const auto boundedRowCount    = std::min(rowCount, _totalRows - firstRow);
    const auto boundedColumnCount = std::min(columnCount, _totalColumns - firstColumn);

    const auto oldFirstRow    = _loadedFirstRow;
    const auto oldRowCount    = _loadedRowCount;
    const auto oldFirstColumn = _loadedFirstColumn;
    const auto oldColumnCount = _loadedColumnCount;

    _loadedFirstRow    = firstRow;
    _loadedRowCount    = boundedRowCount;
    _loadedFirstColumn = firstColumn;
    _loadedColumnCount = boundedColumnCount;
    _values.assign(static_cast<std::size_t>(boundedRowCount * boundedColumnCount), 0.0f);

    std::uint64_t destinationRow    = 0;
    std::uint64_t destinationColumn = 0;
    std::size_t callbackCount       = 0;

    try
    {
        _points->visitRange(firstRow, boundedRowCount, firstColumn, boundedColumnCount, [this, &destinationRow, &destinationColumn, &callbackCount](const PointDataAccessView& view) {
            ++callbackCount;

            for (std::uint64_t row = 0; row < view.rows; ++row)
            {
                for (std::uint64_t column = 0; column < view.columns; ++column)
                {
                    const auto targetRow    = destinationRow + row;
                    const auto targetColumn = destinationColumn + column;

                    if (targetRow < _loadedRowCount && targetColumn < _loadedColumnCount)
                    {
                        _values[static_cast<std::size_t>(targetRow * _loadedColumnCount + targetColumn)] = valueAt(view, row, column);
                    }
                }
            }

            destinationColumn += view.columns;

            if (destinationColumn >= _loadedColumnCount)
            {
                destinationColumn = 0;
                destinationRow += view.rows;
            }
        });
    }
    catch (const std::exception& exception)
    {
        _lastError = QString::fromUtf8(exception.what());
        _values.clear();
        _loadedRowCount = _loadedColumnCount = 0;
        return 0;
    }
    catch (...)
    {
        _lastError = QStringLiteral("Unknown error while loading point-data tiles");
        _values.clear();
        _loadedRowCount = _loadedColumnCount = 0;
        return 0;
    }

    notifyRange(oldFirstRow, oldRowCount, oldFirstColumn, oldColumnCount);
    notifyRange(_loadedFirstRow, _loadedRowCount, _loadedFirstColumn, _loadedColumnCount);

    return callbackCount;
}

void PointsTableModel::setQueryResult(const std::vector<std::uint64_t>& rowIds, const std::vector<std::uint64_t>& columnIds, const std::vector<float>& values)
{
    beginResetModel();

    _queryResult = true;
    _queryRowIds = rowIds;
    _queryColumnIds = columnIds;
    _values = values;
    _lastError.clear();
    _loadedFirstRow = _loadedRowCount = _loadedFirstColumn = _loadedColumnCount = 0;

    endResetModel();
}

void PointsTableModel::notifyRange(const std::uint64_t firstRow, const std::uint64_t rowCount, const std::uint64_t firstColumn, const std::uint64_t columnCount)
{
    if (rowCount == 0 || columnCount == 0)
        return;

    emit dataChanged(index(toModelIndex(firstRow), toModelIndex(firstColumn)), index(toModelIndex(firstRow + rowCount - 1), toModelIndex(firstColumn + columnCount - 1)), { Qt::DisplayRole });
}
