#pragma once

#include "DatasetRef.h"

#include <QMessageBox>

namespace hdps
{

namespace util
{

DatasetRefPrivate::DatasetRefPrivate(const QString& datasetName, QObject* parent /*= nullptr*/) :
    QObject(parent),
    EventListener()
{
    Q_ASSERT(core != nullptr);

    setEventCore(core);
}

void DatasetRefPrivate::setCore(CoreInterface* core)
{
    if (DatasetRefPrivate::core != nullptr)
        QMessageBox::critical(nullptr, QString("HDPS"), "DataSetRefPrivate::core is already assigned", QMessageBox::Ok);

    DatasetRefPrivate::core = core;
}

CoreInterface* DatasetRefPrivate::core = nullptr;

}
}
