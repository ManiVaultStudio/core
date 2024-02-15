// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PointsMetaData.h"

#include <QVariantMap>

using namespace mv;

PointsMetaData::PointsMetaData(QObject* parent, const QString& serializationName /*= "PointsMetaData"*/) :
    DatasetMetaData(parent, serializationName)
{
    auto& metaData = getMetaData();

    metaData["Rows"] = QVariantMap({
        { "Names", QStringList() },
        { "Meta", QVariantList() },
    });

    metaData["Columns"] = QVariantMap({
        { "Names", QStringList() },
        { "Meta", QVariantList() },
    });

    metaData["Overall"] = QVariantMap();
}

void PointsMetaData::resize(int numberOfPoints, int numberOfDimensions)
{
    setNumberOfPoints(numberOfPoints);
    setNumberOfDimensions(numberOfDimensions);
}

QStringList& PointsMetaData::getRowNames()
{
    return *static_cast<QStringList*>(getRowsVariantMap()["Names"].data());
}

QStringList& PointsMetaData::getColumnNames()
{
    return *static_cast<QStringList*>(getColumnsVariantMap()["Names"].data());
}

QVariantMap& PointsMetaData::getOverall()
{
    return *static_cast<QVariantMap*>(getMetaData()["Overall"].data());
}

void PointsMetaData::setNumberOfPoints(int numberOfPoints)
{
    auto& rowNames = getRowNames();

    rowNames.resize(numberOfPoints);

    for (int pointIndex = 0; pointIndex < numberOfPoints; ++pointIndex)
        rowNames[pointIndex] = QString::number(pointIndex);
}

void PointsMetaData::setNumberOfDimensions(int numberOfDimensions)
{
    auto& columnNames = getColumnNames();

    columnNames.resize(numberOfDimensions);

    for (int columnIndex = 0; columnIndex < numberOfDimensions; ++columnIndex)
        columnNames[columnIndex] = QString("Dim %1").arg(QString::number(columnIndex));
}

QVariantMap& PointsMetaData::getRowsVariantMap()
{
    return *static_cast<QVariantMap*>(getMetaData()["Rows"].data());
}

QVariantMap& PointsMetaData::getColumnsVariantMap()
{
    return *static_cast<QVariantMap*>(getMetaData()["Columns"].data());
}
