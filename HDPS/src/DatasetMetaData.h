// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/WidgetAction.h"

namespace mv
{

/**
 * Dataset meta data class
 * 
 * Class for storing and retrieving dataset meta data
 * 
 * @author Thomas Kroes
 */
class DatasetMetaData : public gui::WidgetAction
{
public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title to possibly override
     */
    DatasetMetaData(QObject* parent, const QString& title = "DatasetMetaData");

protected:

    /**
     * Get meta data
     * @return Variant map of all meta data
     */
    QVariantMap& getMetaData();

    /**
     * Get meta data
     * @return Variant map of all meta data
     */
    const QVariantMap& getMetaData() const;

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
