// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <DatasetMetaData.h>

/**
 * Points meta data class
 *
 * Class for storing and retrieving points meta data
 *
 * @author Thomas Kroes
 */
class PointsMetaData : public mv::DatasetMetaData
{
public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title to possibly override
     */
    PointsMetaData(QObject* parent, const QString& serializationName = "PointsMetaData");

    /**
     * Resize to \p numberOfPoints and \p numberOfDimensions
     * @param numberOfPoints Number of points
     * @param numberOfDimensions Number of dimensions
     */
    void resize(int numberOfPoints, int numberOfDimensions);

    /**
     *
     */
    QStringList& getRowNames();

    /**
     * 
     */
    QStringList& getColumnNames();

    /**
     * Get overall meta data
     * @return Variant map for overall data
     */
    QVariantMap& getOverall();

private:

    /**
     * Set number of points to \p numberOfPoints
     * @param numberOfPoints Number of points
     */
    void setNumberOfPoints(int numberOfPoints);

    /**
     * Set number of points to \p numberOfDimensions
     * @param numberOfDimensions Number of dimensions
     */
    void setNumberOfDimensions(int numberOfDimensions);

    QVariantMap& getRowsVariantMap();
    QVariantMap& getColumnsVariantMap();

private:
    QVariantMap     _metaData;  /** Meta data storage in variant map */
};
