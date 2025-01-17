// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "Dataset.h"
#include "StandardItemModel.h"

#include "actions/ToggleAction.h"

#include <QList>
#include <QStandardItem>

namespace mv
{

/**
 * Datasets model class
 *
 * Standard item model class for datasets
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractDatasetsModel : public StandardItemModel
{
    Q_OBJECT

public:

    /** Population */
    enum class PopulationMode {
        Manual,         /** ...done manually with \p setDatasets() */
        Automatic       /** ...performed every time a dataset is added or removed */
    };

public:

    /** Task columns */
    enum class Column {
        Name,               /** Name of the dataset */
        Location,           /** Location of the dataset */
        ID,                 /** Globally unique identifier of the dataset */
        RawDataName,        /** Name of the associated raw data */
        SourceDatasetID,    /** Globally unique dataset identifier of the source dataset (if this dataset is derived) */

        Count
    };

    /** Header strings for several data roles */
    struct ColumHeaderInfo {
        QString     _display;   /** Header string for display role */
        QString     _edit;      /** Header string for edit role */
        QString     _tooltip;   /** Header string for tooltip role */
    };

    /** Column name and tooltip */
    static QMap<Column, ColumHeaderInfo> columnInfo;

protected:

    /** Header standard model item class */
    class HeaderItem : public QStandardItem {
    public:

        /**
         * Construct with \p columHeaderInfo
         * @param columHeaderInfo Column header info
         */
        HeaderItem(const ColumHeaderInfo& columHeaderInfo);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

    private:
        ColumHeaderInfo     _columHeaderInfo;   /** Column header info */
    };

public:

    /** Base standard model item class for a dataset */
    class CORE_EXPORT Item : public QStandardItem, public QObject {
    public:

        /**
         * Construct with reference to the owning \p datasetsModel and smart pointer to the \p dataset
         * @param datasetsModel Reference to the datasets model
         * @param dataset Smart pointer to the dataset to display item for
         * @param editable Whether the model item is editable or not
         */
        Item(AbstractDatasetsModel& datasetsModel, Dataset<DatasetImpl> dataset, bool editable = false);

        /**
         * Get datasets model
         * return Reference to datasets model
         */
        AbstractDatasetsModel& getDatasetsModel();

        /**
         * Get dataset
         * return Reference to smart pointer to dataset to display item for
         */
        Dataset<DatasetImpl>& getDataset();

    private:
        AbstractDatasetsModel&  _datasetsModel;     /** Reference to the owing dataset model */
        Dataset<DatasetImpl>    _dataset;           /** Smart pointer to the dataset to display item for */
    };

    /** Standard model item class for displaying the dataset name */
    class CORE_EXPORT NameItem final : public Item {
    public:

        /**
         * Construct with reference to the owning \p datasetsModel and smart pointer to the \p dataset
         * @param datasetsModel Reference to the owning datasets model
         * @param dataset Smart pointer to the dataset to display item for
         */
        NameItem(AbstractDatasetsModel& datasetsModel, Dataset<DatasetImpl> dataset);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the dataset location */
    class CORE_EXPORT LocationItem final : public Item{
    public:

        /**
         * Construct with reference to the owning \p datasetsModel and smart pointer to the \p dataset
         * @param datasetsModel Reference to the owning datasets model
         * @param dataset Smart pointer to the dataset to display item for
         */
        LocationItem(AbstractDatasetsModel& datasetsModel, Dataset<DatasetImpl> dataset);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the dataset identifier */
    class CORE_EXPORT IdItem final : public Item {
    public:

        /**
         * Construct with reference to the owning \p datasetsModel and smart pointer to the \p dataset
         * @param datasetsModel Reference to the owning datasets model
         * @param dataset Smart pointer to the dataset to display item for
         */
        IdItem(AbstractDatasetsModel& datasetsModel, Dataset<DatasetImpl> dataset);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the raw data name */
    class CORE_EXPORT RawDataNameItem final : public Item {
    public:

        /** No need for custom constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the source dataset identifier name */
    class CORE_EXPORT SourceDatasetIdItem final : public Item {
    public:

        /** No need for custom constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Convenience class for combining dataset items in a row */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct with reference to the owning \p datasetsModel and smart pointer to the \p dataset
         * @param datasetsModel Reference to the owning datasets model
         * @param dataset Smart pointer to the dataset to display item for
         */
        Row(AbstractDatasetsModel& datasetsModel, Dataset<DatasetImpl> dataset) :
            QList<QStandardItem*>()
        {
            append(new NameItem(datasetsModel, dataset));
            append(new LocationItem(datasetsModel, dataset));
            append(new IdItem(datasetsModel, dataset));
            append(new RawDataNameItem(datasetsModel, dataset));
            append(new SourceDatasetIdItem(datasetsModel, dataset));
        }
    };

public:

    /**
     * Construct with \p populationMode and pointer to \p parent object
     * @param populationMode Population mode
     * @param parent Pointer to parent object
     */
    AbstractDatasetsModel(PopulationMode populationMode = PopulationMode::Automatic, QObject* parent = nullptr);

    /**
     * Get index from \p dataset smart pointer
     * @param dataset Dataset to retrieve the index for
     * @return Model index (invalid if not found)
     */
    QModelIndex getIndexFromDataset(Dataset<DatasetImpl> dataset) const;

    /**
     * Get index from \p datasetId
     * @param datasetId Dataset globally unique identifier to retrieve the row index for
     * @return Model index (invalid if not found)
     */
    QModelIndex getIndexFromDataset(const QString& datasetId) const;

    /**
     * Get item from \p datasetId
     * @param datasetId Globally unique identifier of the dataset
     * @return Pointer to found item, nullptr otherwise
     */
    QStandardItem* getItemFromDataset(const QString& datasetId) const;

    /**
     * Get item dataset smart pointer to \p dataset
     * @param dataset Smart pointer to the dataset
     * @return Pointer to found item, nullptr otherwise
     */
    QStandardItem* getItemFromDataset(Dataset<DatasetImpl> dataset) const;

    /** 
     * Get current population mode
     * @return Population mode
     */
    PopulationMode getPopulationMode() const;

    /**
     * Set population mode to \p populationMode
     * @param populationMode Population mode
     */
    void setPopulationMode(PopulationMode populationMode);

    /**
     * Get datasets
     * @return Datasets
     */
    Datasets getDatasets() const;

    /**
     * Get dataset for \p rowIndex
     * @param rowIndex Index of the row to retrieve
     * @return Dataset
     */
    Dataset<DatasetImpl> getDataset(std::int32_t rowIndex) const;

    /**
     * Set the datasets from which can be picked (mode is set to StorageMode::Manual)
     * @param datasets Datasets
     */
    void setDatasets(mv::Datasets datasets);

protected:

    /**
     * Add \p dataset to the model (this method is called when a dataset is added to the manager)
     * @param dataset Smart pointer to dataset to add
     */
    virtual void addDataset(Dataset<DatasetImpl> dataset);

    /**
     * Remove \p dataset from the model (this method is called when a dataset is about to be removed from the manager)
     * @param dataset Smart pointer to dataset to remove
     */
    virtual void removeDataset(Dataset<DatasetImpl> dataset) final;

public: // Action getters

    gui::ToggleAction& getShowIconAction() { return _showIconAction; }

private:
    PopulationMode      _populationMode;    /** Population mode (e.g. manual or automatic) */
    gui::ToggleAction   _showIconAction;    /** Whether to show the dataset icon */
    Datasets            _datasets;          /** Datasets in the model */

    friend class Item;
};

}
