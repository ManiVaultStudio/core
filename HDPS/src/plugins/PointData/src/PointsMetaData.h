// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <DatasetMetaData.h>

class PointsMetaData : public mv::DatasetMetaData
{
public:

    /**
     * Construct with \p serialization name
     * @param serializationName Serialization to possibly override
     */
    PointsMetaData(const QString& serializationName = "PointsMetaData");

public: // Serialization

    /**
     * Load from variant map
     * @param variantMap Variant map
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save to variant map
     * @return Variant map
     */
    QVariantMap toVariantMap() const override;

private:
    QVariantMap     _metaData;      /** Meta data storage in variant map */
};
