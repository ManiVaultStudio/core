// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "Dataset.h"

#include <QMimeData>

namespace mv {

/**
 * Datasets mime data class
 * 
 * Class for drag-and-drop of one or more datasets
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT DatasetsMimeData final : public QMimeData
{
public:

    /**
     * Construct with \p datasets
     * @param datasets Mime data datasets for dropping
     */
    DatasetsMimeData(Datasets datasets);

    /**
     * Get supported mime types
     * @return List of string of supported mime types
     */
    QStringList formats() const override;

    /**
     * Get stored datasets
     * @return Stored datasets
     */
    Datasets getDatasets() const;

    /** Supported mime type */
    static QString format();

private:
    Datasets    _datasets;     /** Mime data datasets */
};

}
