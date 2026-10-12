#include "DataType.h"

#include <QDebug>

#include <unordered_set>

namespace {
bool require(bool condition, const char* message)
{
    if (!condition)
        qCritical().noquote() << message;
    return condition;
}
}

bool preservesDataTypeValue()
{
    const mv::DataType imageType("image");
    const mv::DataType sameImageType("image");
    const mv::DataType tableType("table");

    return require(imageType.getTypeString() == "image", "data type string was not preserved")
        && require(imageType == sameImageType, "equal data types were not equal")
        && require(imageType != tableType, "different data types were equal")
        && require(QString("prefix:") + imageType == "prefix:image", "data type string concatenation was incorrect");
}

bool supportsHashBasedLookup()
{
    std::unordered_set<mv::DataType> dataTypes;
    dataTypes.emplace("image");
    dataTypes.emplace("table");

    return require(dataTypes.find(mv::DataType("image")) != dataTypes.end(), "equal data type was not found in hash set")
        && require(dataTypes.find(mv::DataType("missing")) == dataTypes.end(), "missing data type was found in hash set");
}

int main()
{
    return preservesDataTypeValue()
        && supportsHashBasedLookup() ? 0 : 1;
}
