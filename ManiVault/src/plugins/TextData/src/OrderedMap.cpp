#include "OrderedMap.h"

#include "util/Serialization.h"
#include "util/Timer.h"

#include <stdexcept>

using namespace mv::util;

namespace
{
    template <class T>
    QVariantList toVariantList(std::vector<T>& vec)
    {
        QVariantList variantList;

        for (const T& t : vec)
        {
            variantList.push_back(t);
        }
        return variantList;
    }

    template <class T>
    QVariantMap stdToVariantMap(const std::unordered_map<QString, T>& map)
    {
        QVariantMap variantMap;

        for (auto& kv : map)
        {
            variantMap[kv.first] = QVariant::fromValue(kv.value);
        }

        return variantMap;
    }

    // More clean storage of this data structure by combining all vectors into one big one
    template <>
    QVariantMap stdToVariantMap(const std::unordered_map<QString, std::vector<QString>>& map)
    {
        if (map.empty())
            return QVariantMap();

        // Iterate over all key-value pairs and add them to a big stringlist
        QStringList combinedList;
        for (auto& kv : map)
        {
            combinedList.push_back(kv.first);
            combinedList.push_back(QString::number(kv.second.size()));
            for (auto& v : kv.second)
            {
                combinedList.push_back(v);
            }
        }
        qDebug() << combinedList;
        QVariantMap variantMap = storeOnDisk(combinedList);

        return variantMap;
    }

    template <class T>
    void stdFromVariantMap(const QVariantMap& variantMap, std::unordered_map<QString, T>& map)
    {
        QMapIterator<QString, T> it(map);
        while (it.hasNext())
        {
            it.next();
            map[it.key()] = it.value();
        }
    }

    // Load a long combined list of this data structure into its original form
    template <>
    void stdFromVariantMap(const QVariantMap& variantMap, std::unordered_map<QString, std::vector<QString>>& map)
    {
        QStringList combinedList;
        loadFromDisk(variantMap, combinedList);

        // Iterate over the big stringlist and extract the key-value pairs
        for (qsizetype i = 0; i < combinedList.size(); )
        {
            QString key = combinedList[i];                          // Extract key
            size_t numElements = combinedList[i + 1].toULongLong(); // Extract number of rows in value
            // Extract value
            std::vector<QString> value(numElements);
            for (size_t j = 0; j < numElements; j++)
            {
                value[j] = combinedList[i + 2 + j];
            }

            // Put key and value pair into map
            map[key] = value;

            i += 2 + numElements;
        }
    }
}

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

    bool OrderedMap::hasColumn(QString columnName) const
    {
        return _columns.find(columnName) != _columns.end();
    }

    const std::vector<QString>& OrderedMap::getColumn(QString columnName) const
    {
        try
        {
            return _columns.at(columnName);
        }
        catch ([[maybe_unused]] std::out_of_range& e)
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

    // Serialization
    void OrderedMap::fromVariantMap(const QVariantMap& variantMap)
    {
        Timer t("OrderedMap::fromVariantMap");

        Serializable::fromVariantMap(variantMap);

        variantMapMustContain(variantMap, "NumRows");
        variantMapMustContain(variantMap, "ColumnHeaders");
        variantMapMustContain(variantMap, "table-columns");

        _numRows = variantMap["NumRows"].value<size_t>();

        const auto columnHeadersMap = variantMap["ColumnHeaders"].toMap();
        QStringList columnHeaders;
        loadFromDisk(columnHeadersMap, columnHeaders);
        for (QString header : columnHeaders)
        {
            _columnHeaders.push_back(header);
        }

        stdFromVariantMap(variantMap["table-columns"].toMap(), _columns);
    }

    QVariantMap OrderedMap::toVariantMap() const
    {
        auto variantMap = Serializable::toVariantMap();
        
        QStringList columnHeaders;
        for (QString header : _columnHeaders)
        {
            columnHeaders.push_back(header);
        }

        QVariantMap columnsMap = stdToVariantMap(_columns);

        variantMap.insert({
            { "NumRows", QVariant::fromValue(_numRows) },
            { "ColumnHeaders", QVariant::fromValue(storeOnDisk(columnHeaders)) },
            { "table-columns", QVariant::fromValue(columnsMap) }
        });

        return variantMap;
    }
}
