// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "util/Serializable.h"

namespace mv
{

class DatasetMetaData : public util::Serializable
{
public:

    /**
     * Construct with \p serialization name
     * @param serializationName Serialization to possibly override
     */
    DatasetMetaData(const QString& serializationName = "DatasetMetaData");

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

}
