// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"
#include "Dataset.h"

#include "actions/ToggleAction.h"

class QSortFilterProxyModel;

namespace mv
{

class DatasetImpl;
class DatasetsListModel;

namespace plugin {
    class RawData;
}

/**
 * Abstract data manager
 *
 * Base abstract data manager class for managing datasets.
 *
 * @author Thomas Kroes and Julian Thijssen
 */
class CORE_EXPORT AbstractDataManager : public AbstractManager
{
    Q_OBJECT

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    explicit AbstractDataManager(QObject* parent) :
        AbstractManager(parent, "Data")
    {
    }

protected: // Raw data

    /**
     * Add \p rawData to the manager (plugin manager remains the owner)
     * @param rawData Pointer to the raw data to add
     */
    virtual void addRawData(plugin::RawData* rawData) = 0;

    /**
     * Remove raw data with \p rawDataName from the manager if there are no more (selection) datasets referencing it
     * @param rawDataName Name of the raw data to remove
     */
    virtual void removeRawData(const QString& rawDataName) = 0;

protected: // Raw data

    /**
     * Get raw data by \p rawDataName
     * @param rawDataName Name of the raw data
     * @return Pointer to raw data if found, nullptr otherwise
     */
    virtual plugin::RawData* getRawData(const QString& rawDataName) = 0;

    /**
     * Get raw data by \p rawDataName
     * @param rawDataName Name of the raw data
     * @return Pointer of \p RawDataType to raw data if found, nullptr otherwise
     */
    template<typename RawDataType>
    inline RawDataType* getRawData(const QString& rawDataName) {
        return dynamic_cast<RawDataType*>(getRawData(rawDataName));
    }

public: // Raw data

    /**
     * Get raw data names
     * @retun Raw data names list
     */
    virtual QStringList getRawDataNames() const = 0;

    /**
     * Get raw data size by \p rawDataName
     * @param rawDataName Name of the raw data
     * @return Size of the raw data in bytes
     */
    virtual std::uint64_t getRawDataSize(const QString& rawDataName) const = 0;

    /**
     * Get overall raw data size
     * @return Overall raw data size in bytes
     */
    std::uint64_t getOverallRawDataSize() const {
        std::uint64_t overallRawDataSize = 0;

        for (const auto& rawDataName : getRawDataNames())
            overallRawDataSize += getRawDataSize(rawDataName);

        return overallRawDataSize;
    };

    /**
     * Get raw data type string by \p rawDataName
     * @param rawDataName Name of the raw data
     * @return Raw data type string
     */
    virtual QString getRawDataType(const QString& rawDataName) const = 0;

public: // Dataset creation

    /**
     * Creates dataset of \p kind with \p guiName to the manager and returns the created dataset
     * @param kind Kind of data plugin
     * @param guiName Name of the added dataset in the GUI
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (will add to the root of the data hierarchy if not valid)
     * @param id Globally unique dataset identifier (use only for deserialization)
     * @param notify Whether to notify the core that a dataset is added
     * @return Smart pointer to the create dataset
     */
    virtual Dataset<DatasetImpl> createDataset(const QString& kind, const QString& guiName, const Dataset<DatasetImpl>& parentDataset = Dataset<DatasetImpl>(), const QString& id = "", bool notify = true) = 0;

    /**
     * Creates dataset without selection of \p kind with \p guiName to the manager and returns the created dataset (this is only used fore serialization purposes)
     * @param kind Kind of data plugin
     * @param guiName Name of the added dataset in the GUI
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (will add to the root of the data hierarchy if not valid)
     * @param id Globally unique dataset identifier (use only for deserialization)
     * @param notify Whether to notify the core that a dataset is added
     * @return Smart pointer to the create dataset
     */
    virtual Dataset<DatasetImpl> createDatasetWithoutSelection(const QString& kind, const QString& guiName, const Dataset<DatasetImpl>& parentDataset = Dataset<DatasetImpl>(), const QString& id = "", bool notify = true) = 0;

    /**
     * Creates dataset of \p kind with \p guiName to the manager and returns the created dataset as \p DatasetType
     * @param kind Kind of data plugin
     * @param guiName Name of the added dataset in the GUI
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (will add to the root of the data hierarchy if not valid)
     * @param id Globally unique dataset identifier (use only for deserialization)
     * @param notify Whether to notify the core that a dataset is added
     * @return Smart pointer to the added dataset
     */
    template <class DatasetType>
    inline Dataset<DatasetType> createDataset(const QString& kind, const QString& dataSetGuiName, const Dataset<DatasetImpl>& parentDataset = Dataset<DatasetImpl>(), const QString& id = "", bool notify = true) {
        return Dataset<DatasetType>(createDataset(kind, dataSetGuiName, parentDataset, id, notify).get<DatasetType>());
    }

protected: // Dataset add

    /**
     * Adds \p dataset to the manager
     * @param dataset Dataset to added
     * @param parentDataset Parent dataset (if any)
     * @param notify Whether to notify the core that a dataset is added
     */
    virtual void addDataset(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, bool notify = true) = 0;

public: // Dataset remove

    /**
     * Remove \p dataset from the manager
     * @param dataset Smart pointer to dataset to remove
     */
    virtual void removeDataset(Dataset<DatasetImpl> dataset) = 0;

    /**
     * Remove \p datasets from the manager
     * @param datasets Smart pointer to datasets to remove
     */
    virtual void removeDatasets(Datasets datasets) = 0;

    /**
     * Remove datasets that reference \p rawDataName from the manager
     * @param @param rawDataName Name of the raw data
     */
    virtual void removeDatasets(const QString& rawDataName) = 0;

public: // Derived datasets

    /**
     * Creates derived dataset from \666p sourceDataset with \p guiName and returns the created derived dataset
     * @param guiName GUI name for the new dataset from the core
     * @param sourceDataset Smart pointer to the source dataset from which this dataset will be derived
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (will attach to source dataset in hierarchy if not valid)
     * @param notify Whether to notify the core that a dataset is added
     * @return Smart pointer to the created derived dataset
     */
    virtual Dataset<DatasetImpl> createDerivedDataset(const QString& guiName, const Dataset<DatasetImpl>& sourceDataset, const Dataset<DatasetImpl>& parentDataset = Dataset<DatasetImpl>(), bool notify = true) = 0;

    /**
     * Creates derived dataset from \p sourceDataset with \p guiName and returns the created derived dataset as \p DatasetType
     * @param guiName GUI name for the new dataset from the core
     * @param sourceDataset Smart pointer to the source dataset from which this dataset will be derived
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (will attach to source dataset in hierarchy if not valid)
     * @param notify Whether to notify the core that a dataset is added
     * @return Smart pointer to the created derived dataset of \p DatasetType
     */
    template <typename DatasetType>
    inline Dataset<DatasetType> createDerivedDataset(const QString& guiName, const Dataset<DatasetImpl>& sourceDataset, const Dataset<DatasetImpl>& parentDataset = Dataset<DatasetImpl>(), bool notify = true) {
        return createDerivedDataset(guiName, sourceDataset, parentDataset, notify);
    }

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
    virtual Dataset<DatasetImpl> createSubsetFromSelection(const Dataset<DatasetImpl>& selection, const Dataset<DatasetImpl>& sourceDataset, const QString& guiName, const Dataset<DatasetImpl>& parentDataset, const bool& visible = true, bool notify = true) = 0;

    /**
     * Creates a copy of \p selection with the \p sourceDataset, adds it to the manager and returns the created subset as \p DatasetType
     * @param selection Smart pointer to the selection set
     * @param sourceDataset Smart pointer to the source dataset
     * @param guiName GUI name of the subset
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (will attach to source dataset in hierarchy if not valid)
     * @param visible Whether the new dataset is visible in the data hierarchy
     * @param notify Whether to notify the core that a dataset is added
     * @return Smart pointer to the created subset of \p DatasetType
     */
    template <typename DatasetType>
    inline Dataset<DatasetType> createSubsetFromSelection(const Dataset<DatasetImpl>& selection, const Dataset<DatasetImpl>& sourceDataset, const QString& guiName, const Dataset<DatasetImpl>& parentDataset, const bool& visible = true, bool notify = true) {
        return createSubsetFromSelection(selection, sourceDataset, guiName, parentDataset, visible);
    }

public: // Dataset access

    /**
     * Get dataset by \p datasetId
     * @param datasetId Globally unique identifier of the dataset
     * @return Smart pointer to the dataset
     */
    virtual Dataset<DatasetImpl> getDataset(const QString& datasetId) = 0;

    /**
     * Get dataset by \p datasetId as \p DatasetType
     * @param datasetId Globally unique identifier of the dataset
     * @return Smart pointer to the dataset of \p DatasetType
     */
    template<typename DatasetType>
    inline Dataset<DatasetType> getDataset(const QString& datasetId) {
        return Dataset<DatasetType>(getDataset(datasetId));
    }

    /**
     * Get all datasets for \p dataTypes
     * @param dataTypes Data types filter (if empty returns all types)
     * @return Smart pointers to datasets
     */
    virtual Datasets getAllDatasets(const std::vector<DataType>& dataTypes = std::vector<DataType>()) const = 0;

protected: // Selection

    /**
     * Add \p selection on \p rawDataName to the manager
     * @param rawDataName Name of the raw data
     * @param selection Smart pointer to selection dataset
     */
    virtual void addSelection(const QString& rawDataName, Dataset<DatasetImpl> selection) = 0;

public: // Selection

    /**
     * Get selection dataset by \p rawDataName
     * @param rawDataName Name of the raw data
     * @return Smart pointer to the selection dataset
     */
    virtual Dataset<DatasetImpl> getSelection(const QString& rawDataName) = 0;

    /**
     * Get a selection of \p DatasetType by \p rawDataName
     * @param rawdataName Name of the raw data
     * @return Smart pointer to the selection dataset
     */
    template<typename DatasetType>
    inline Dataset<DatasetType> getSelection(const QString& rawDataName) {
        return Dataset<DatasetType>(dynamic_cast<DatasetType*>(getSelection(rawDataName).get()));
    }

    /**
     * Get all selection datasets
     * @return Smart pointers to selection datasets
     */
    virtual Datasets getAllSelections() = 0;

protected: // Selection

    /**
     * Removes selection of which the raw data name matches \p rawDataName
     * @param rawDataName Name of the raw data
     */
    virtual void removeSelection(const QString& rawDataName) = 0;

public: // Dataset and selection grouping

    /**
     * Groups \p datasets into one dataset and returns the group dataset
     * @param datasets Two or more datasets to group
     * @param guiName Name of the created dataset in the GUI (if empty, the user will be prompted for a name)
     * @return Smart pointer to created group dataset
     */
    virtual Dataset<DatasetImpl> groupDatasets(const Datasets& datasets, const QString& guiName = "") = 0;

    /**
     * Get selection grouping setEnabled action
     * @return Pointer to selection grouping toggle action (only valid if there is a current project)
     */
    virtual gui::ToggleAction* getSelectionGroupingAction() = 0;

public: // Model

    /**
     * Get datasets list model
     * @return Reference to datasets list model
     */
    virtual const DatasetsListModel& getDatasetsListModel() const = 0;

    /**
     * Link \p filterModel to the datasets list model
     * @param filterModel Pointer to filter model
     */
    virtual void linkFilterModelToDatasetsListModel(QSortFilterProxyModel* filterModel) = 0;

signals:

    /**
     * Signals that \p rawData is added to the data manager
     * @param rawData Pointer to raw data that was added
     */
    void rawDataAdded(plugin::RawData* rawData);

    /**
     * Signals that \p rawData is about to be removed from the data manager
     * @param rawData Pointer to raw data that is about to be removed
     */
    void rawDataAboutToBeRemoved(plugin::RawData* rawData);

    /**
     * Signals that raw data with \p rawDataId is removed from the data manager
     * @param rawData Pointer to raw data that was added
     */
    void rawDataRemoved(const QString& rawDataId);

    /**
     * Signals that \p dataset is added to the data manager
     * @param dataset Dataset that was added
     * @param parentDataset Parent dataset (if any)
     * @param visible Whether \p dataset should be visible or not
     */
    void datasetAdded(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, bool visible = true);

    /**
     * Signals that \p dataset is about to be removed from the data manager
     * @param dataset Dataset which is about to be removed
     */
    void datasetAboutToBeRemoved(Dataset<DatasetImpl> dataset);

    /**
     * Signals that dataset with \p datasetId is removed from the data manager
     * @param datasetId GUID of the removed dataset
     */
    void datasetRemoved(const QString& datasetId);

    /**
     * Signals that \p selection dataset is added to the data manager
     * @param selection Selection dataset that was added
     */
    void selectionAdded(Dataset<DatasetImpl> selection);

    /**
     * Signals that \p selection dataset is about to be removed from the data manager
     * @param selection Selection dataset that is about to be removed
     */
    void selectionAboutToBeRemoved(Dataset<DatasetImpl> selection);

    /**
     * Signals that selection dataset with \p selectionId is removed from the data manager
     * @param selectionId GUID of the removed selection dataset
     */
    void selectionRemoved(const QString& selectionId);

    friend class PluginManager;
    friend class DatasetImpl;
    friend class RawDataModel;
};

}