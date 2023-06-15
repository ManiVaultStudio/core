#pragma once

#include "Dataset.h"

#include <QMimeData>

namespace hdps {

/**
 * Dataset mime data class
 * 
 * Class for drag-and-drop of a dataset
 * 
 * @author Thomas Kroes
 */
class DatasetMimeData final : public QMimeData
{
public:

    /**
     * Construct with \p dataset
     * @param dataset Pointer to dataset for dropping
     */
    DatasetMimeData(Dataset<DatasetImpl> dataset);

    /** Destructor */
    ~DatasetMimeData();

    /**
     * Get supported mime types
     * @return List of string of supported mime types
     */
    QStringList formats() const override;

    /**
     * Get stored dataset
     * @return Pointer to stored dataset
     */
    Dataset<DatasetImpl> getDataset() const;

    /** Supported mime type */
    static QString format();

private:
    Dataset<DatasetImpl>    _dataset;       /** Pointer to mime data dataset */
};

}