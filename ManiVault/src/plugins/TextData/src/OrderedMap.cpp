#include "OrderedMap.h"

#include <stdexcept>

namespace mv
{
    OrderedMap::OrderedMap() :
        _numRows(-1)
    {

    }

    size_t OrderedMap::getNumRows() const
    {
        return _numRows;
    }

    size_t OrderedMap::getNumColumns() const
    {
        return _columnHeaders.size();
    }

    const std::vector<QString>& OrderedMap::getColumnNames() const
    {
        return _columnHeaders;
    }

    const std::vector<QString>& OrderedMap::getColumn(QString columnName) const
    {
        try
        {
            return _columns.at(columnName);
        }
        catch (std::out_of_range& e)
        {
            qWarning() << "Failed to find column: " << columnName;
            return _columns.at(columnName);
        }
    }

    void OrderedMap::addColumn(QString columnName, std::vector<QString>& columnData)
    {
        // If this is the first column added, set the number of rows for future adds
        if (_numRows == -1)
            _numRows = columnData.size();
        else
        {
            // Check if columnData has the same number of rows as we expect
            Q_ASSERT(columnData.size() == _numRows);
            if (columnData.size() != _numRows)
            {
                qWarning() << "[WARNING] Trying to add a column of size: " << columnData.size() << " but previous columns have size: " << _numRows << ", refusing to add this column.";
                return;
            }
        }

        // Add the column name to the headers
        _columnHeaders.push_back(columnName);
        // Add the column
        _columns[columnName] = columnData;
    }
}
