#include "DatasetMimeData.h"

namespace hdps {

DatasetMimeData::DatasetMimeData(Dataset<DatasetImpl> dataset) :
    QMimeData(),
    _dataset(dataset)
{
}

DatasetMimeData::~DatasetMimeData()
{
}

QStringList DatasetMimeData::formats() const
{
    return { format() };
}

hdps::Dataset<hdps::DatasetImpl> DatasetMimeData::getDataset() const
{
    return _dataset;
}

QString DatasetMimeData::format()
{
    return "application/dataset";
}

}