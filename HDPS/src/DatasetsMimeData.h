#pragma once

#include "Dataset.h"

#include <QMimeData>

namespace hdps {

/**
 * Datasets mime data class
 * 
 * Class for drag-and-drop of one or more datasets
 * 
 * @author Thomas Kroes
 */
class DatasetsMimeData final : public QMimeData
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