#include "PluginFactory.h"

#include "Set.h"

namespace hdps
{
namespace plugin
{

QStringList PluginFactory::getDatasetTypes(const Datasets& datasets) const
{
    QStringList datasetTypes;

    for (auto dataset : datasets)
        datasetTypes << dataset->getDataType().getTypeString();

    return datasetTypes;
}

bool PluginFactory::areAllDatasetsOfTheSameType(const Datasets& datasets, const QString& datasetType) const
{
    for (auto dataset : datasets)
        if (dataset->getDataType().getTypeString() != datasetType)
            return false;

    return true;
}

}

}