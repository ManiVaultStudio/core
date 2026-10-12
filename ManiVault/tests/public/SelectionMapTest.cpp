#include "LinkedData.h"

#include <QDebug>

namespace {
bool require(bool condition, const char* message)
{
    if (!condition)
        qCritical().noquote() << message;
    return condition;
}
}

bool supportsIndexedMappings()
{
    mv::SelectionMap selectionMap;
    selectionMap.getMap()[7] = { 11, 13 };

    mv::SelectionMap::Indices indices;
    selectionMap.populateMappingIndices(7, indices);

    return require(selectionMap.hasMappingForPointIndex(7), "indexed mapping was not found")
        && require(!selectionMap.hasMappingForPointIndex(8), "missing indexed mapping was found")
        && require(indices == mv::SelectionMap::Indices({ 11, 13 }), "indexed mapping values were incorrect");
}

bool supportsImagePyramidMappings()
{
    const mv::SelectionMap selectionMap(QSize(2, 2), QSize(4, 4));
    mv::SelectionMap::Indices indices;
    selectionMap.populateMappingIndices(1, indices);

    return require(selectionMap.hasMappingForPointIndex(3), "image-pyramid boundary mapping was not found")
        && require(!selectionMap.hasMappingForPointIndex(4), "out-of-range image-pyramid mapping was found")
        && require(indices == mv::SelectionMap::Indices({ 2, 6, 3, 7 }), "image-pyramid mapping values were incorrect");
}

int main()
{
    return supportsIndexedMappings()
        && supportsImagePyramidMappings() ? 0 : 1;
}
