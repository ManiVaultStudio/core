// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Set.h"

#include <AbstractDataManager.h>

#include <map>
#include <unordered_map>
#include <memory>
#include <exception>
#include <functional>

namespace mv
{

namespace plugin {
    class RawData;
}

/**
 * Data manager
 *
 * Concrete implementation of the AbstractDataManager
 * 
 * Provides storage place for:
 * - Datasets
 * - Raw data
 * - Selections
 * 
 * Main purpose is to add, remove and retrieve datasets
 *
 * @author Thomas Kroes and Julian Thijssen
 */
class DataManager final : public mv::AbstractDataManager
{
    Q_OBJECT

public:

    /**
     * Construct data manager with \p parent object
     * @param parent Pointer to parent object
     */
    DataManager(QObject* parent = nullptr);

    /** Reset the data manager upon destruction */
    ~DataManager();

    /** Perform manager startup initialization */
    void initialize() override;

    /** Resets the contents of the data manager */
    void reset() override;

protected: // Raw data

    /**
     * Add \p rawData to the manager (plugin manager remains the owner)
     * @param rawData Pointer to the raw data to add
     */
    void addRawData(plugin::RawData* rawData) override;

    /**
     * Remove raw data with \p rawDataName from the manager if there are no more (selection) datasets referencing it
     * @param rawDataName Name of the raw data to remove
     */
    void removeRawData(const QString& rawDataName) override;

public: // Raw data

    /**
     * Get raw data by \p rawDataName
     * @param rawDataName Name of the raw data
     * @return Pointer to raw data if found, nullptr otherwise
     */
    plugin::RawData* getRawData(const QString& rawDataName) override;

    /**
     * Get raw data names
     * @retun Raw data names list
     */
    QStringList getRawDataNames() const override;

public: // Datasets

    /**
     * Creates dataset of \p kind with \p guiName to the manager and returns the created dataset
     * @param kind Kind of data plugin
     * @param guiName Name of the added dataset in the GUI
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (will add to the root of the data hierarchy if not valid)
     * @param id Globally unique dataset identifier (use only for deserialization)
     * @param notify Whether to notify the core that a dataset is added
     * @return Smart pointer to the create dataset
     */
    Dataset<DatasetImpl> createDataset(const QString& kind, const QString& guiName, const Dataset<DatasetImpl>& parentDataset = Dataset<DatasetImpl>(), const QString& id = "", bool notify = true) override;

protected: // Dataset remove

    /**
     * Add \p dataset to the manager
     * @param dataset Dataset to added
     * @param parentDataset Parent dataset (if any)
     * @param notify Whether to notify the core that a dataset is added
     */
    void addDataset(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, bool notify = true) override;

public: // Dataset remove

    /**
     * Remove \p dataset from the manager
     * @param dataset Smart pointer to dataset to remove
     */
    void removeDataset(Dataset<DatasetImpl> dataset) override;

    /**
     * Remove \p datasets from the manager
     * @param datasets Smart pointer to datasets to remove
     */
    void removeDatasets(Datasets datasets) override;

    /**
     * Remove datasets that reference \p rawDataName from the manager
     * @param @param rawDataName Name of the raw data
     */
    void removeDatasets(const QString& rawDataName) override;

public:// Derived datasets

    /**
     * Creates derived dataset from \p sourceDataset with \p guiName and returns the created derived dataset
     * @param guiName GUI name for the new dataset from the core
     * @param sourceDataset Smart pointer to the source dataset from which this dataset will be derived
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (will attach to source dataset in hierarchy if not valid)
     * @param notify Whether to notify the core that a dataset is added
     * @return Smart pointer to the created derived dataset
     */
    Dataset<DatasetImpl> createDerivedDataset(const QString& guiName, const Dataset<DatasetImpl>& sourceDataset, const Dataset<DatasetImpl>& parentDataset = Dataset<DatasetImpl>(), bool notify = true) override;

public: // Subsets

    /**
     * Creates a copy of \p selection with the \p sourceDataset, adds it to the manager and returns the created subset
     * @param selection Smart pointer to the selection set
     * @param sourceDataset Smart pointer to the source dataset
     * @param guiName GUI name of the subset
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (will attach to source dataset in hierarchy if not valid)
     * @param visible Whether the new dataset is visible in the data hierarchy
     * @param notify Whether to notify the core that a dataset is added
     * @return Smart pointer to the created subset
     */
    Dataset<DatasetImpl> createSubsetFromSelection(const Dataset<DatasetImpl>& selection, const Dataset<DatasetImpl>& sourceDataset, const QString& guiName, const Dataset<DatasetImpl>& parentDataset, const bool& visible = true, bool notify = true) override;

public: // Dataset access

    /**
     * Get dataset by \p datasetId
     * @param datasetId Globally unique identifier of the dataset
     * @return Smart pointer to the dataset
     */
    Dataset<DatasetImpl> getDataset(const QString& datasetId) override;

    /**
     * Get all datasets for \p dataTypes
     * @param dataTypes Data types filter (if empty returns all types)
     * @return Smart pointers to datasets
     */
    Datasets getAllDatasets(const std::vector<DataType>& dataTypes = std::vector<DataType>()) const override;

protected: // Selection

    /**
     * Add \p selection on \p rawDataName to the manager
     * @param rawDataName Name of the raw data
     * @param selection Smart pointer to selection dataset
     */
    void addSelection(const QString& rawDataName, Dataset<DatasetImpl> selection) override;

public: // Selection

    /**
     * Get selection by \p rawDataName
     * @param rawDataName Name of the raw data
     * @return Smart pointer to the selection dataset
     */
    Dataset<DatasetImpl> getSelection(const QString& rawDataName) override;

    /**
     * Get all selection datasets
     * @return Smart pointers to selection datasets
     */
    Datasets getAllSelections() override;

protected:

    /**
     * Removes all selections of which the raw data name matches \p rawDataName
     * @param rawDataName Name of the raw data
     */
    void removeSelections(const QString& rawDataName) override;

public: // Dataset and selection grouping

    /**
     * Groups \p datasets into one dataset and returns the group dataset
     * @param datasets Two or more datasets to group
     * @param guiName Name of the created dataset in the GUI (if empty, the user will be prompted for a name)
     * @return Smart pointer to created group dataset
     */
    Dataset<DatasetImpl> groupDatasets(const Datasets& datasets, const QString& guiName = "") override;

    /**
     * Get selection grouping toggle action
     * @return Reference to selection grouping toggle action
     */
    gui::ToggleAction& getSelectionGroupingAction() override;

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

private:
    std::unordered_map<QString, plugin::RawData*>   _rawDataMap;                /** Maps raw data name to raw data plugin shared pointer (the plugins are owned by the plugin manager) */
    std::vector<std::unique_ptr<DatasetImpl>>       _datasets;                  /** Vector of pointers to datasets */
    std::vector<std::unique_ptr<DatasetImpl>>       _selections;                /** Vector of pointers to selection datasets */
    gui::ToggleAction                               _selectionGroupingAction;   /** Action for toggling whether dataset selection groupin is enabled or not */
};

}
