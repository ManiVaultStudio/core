#pragma once

#include "DatasetSignals.h"

namespace hdps
{

void DatasetSignals::notifyChanged(DatasetImpl* dataset)
{
    emit changed(dataset);
}

void DatasetSignals::notifyDataChanged()
{
    emit dataChanged();
}

void DatasetSignals::notifyDataAboutToBeRemoved()
{
    emit dataAboutToBeRemoved();
}

void DatasetSignals::notifyDataRemoved(const QString& datasetId)
{
    emit dataRemoved(datasetId);
}

void DatasetSignals::notifyDataGuiNameChanged(const QString& oldGuiName, const QString& newGuiName)
{
    emit dataGuiNameChanged(oldGuiName, newGuiName);
}

}
