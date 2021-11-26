#pragma once

#include "Dataset.h"
#include "Set.h"

#include "actions/WidgetAction.h"
#include "actions/OptionAction.h"

#include <QAbstractListModel>

using namespace hdps::gui;
using namespace hdps::util;

/**
 * Dataset picker action class
 *
 * Action class for picking a dataset from a list
 * Automatically removes items when datasets are removed and renamed
 *
 * @author Thomas Kroes
 */
class DatasetPickerAction : public OptionAction, public hdps::EventListener
{
Q_OBJECT

protected:

    /**
     * Datasets list model class
     *
     * List model class which serves as input to the dataset picker option action class
     *
     * @author Thomas Kroes
     */
    class DatasetsModel : public QAbstractListModel {
    public:

        /** (Default) constructor */
        DatasetsModel(QObject* parent = nullptr);

        /**
         * Get the number of row
         * @param parent Parent model index
         * @return Number of rows in the model
         */
        int rowCount(const QModelIndex& parent = QModelIndex()) const;

        /**
         * Get the row index of a dataset
         * @param parent Parent model index
         * @return Row index of the dataset
         */
        int rowIndex(const hdps::Dataset<hdps::DatasetImpl>& dataset) const;

        /**
         * Get the number of columns
         * @param parent Parent model index
         * @return Number of columns in the model
         */
        int columnCount(const QModelIndex& parent = QModelIndex()) const;

        /**
         * Get data
         * @param index Model index to query
         * @param role Data role
         * @return Data
         */
        QVariant data(const QModelIndex& index, int role) const;

        /**
         * Get datasets
         * @return Vector of smart pointers to datasets
         */
        const QVector<hdps::Dataset<hdps::DatasetImpl>>& getDatasets() const;

        /**
         * Get dataset at the specified row index
         * @param rowIndex Index of the row
         * @return Smart pointer to dataset
         */
        hdps::Dataset<hdps::DatasetImpl> getDataset(const std::int32_t& rowIndex) const;

        /**
         * Set datasets (resets the model)
         * @param datasets Vector of smart pointers to datasets
         */
        void setDatasets(const QVector<hdps::Dataset<hdps::DatasetImpl>>& datasets);

        /**
         * Remove specific dataset
         * @param dataset Smart pointer to dataset
         */
        void removeDataset(const hdps::Dataset<hdps::DatasetImpl>& dataset);

        /** Get whether to show the full path name in the GUI */
        bool getShowFullPathName() const;

        /**
         * Set whether to show the full path name in the GUI
         * @param showFullPathName Whether to show the full path name in the GUI
         */
        void setShowFullPathName(const bool& showFullPathName);

        /** Updates the model from the datasets */
        void updateData();

    protected:
        QVector<hdps::Dataset<hdps::DatasetImpl>>   _datasets;              /** Datasets from which can be picked */
        bool                                        _showFullPathName;      /** Whether to show the full path name in the GUI */
    };

public:

    /** 
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    DatasetPickerAction(QObject* parent, const QString& title);

    /**
     * Set the datasets from which can be picked
     * @param datasets Datasets from which can be picked
     */
    void setDatasets(const QVector<hdps::Dataset<hdps::DatasetImpl>>& datasets);

    /** Get the current dataset */
    hdps::Dataset<hdps::DatasetImpl> getCurrentDataset() const;

    /** Get the current dataset */
    template<typename DatasetType>
    hdps::Dataset<DatasetType> getCurrentDataset() const
    {
        return getCurrentDataset();
    }

    /**
     * Set the current dataset
     * @param currentDataset Current dataset reference
     */
    void setCurrentDataset(const hdps::Dataset<hdps::DatasetImpl>& currentDataset);

signals:

    /**
     * Signals that a dataset has been picked
     * @param Dataset reference to picked dataset
     */
    void datasetPicked(const hdps::Dataset<hdps::DatasetImpl>& pickedDataset);

protected:
    DatasetsModel       _datasetsModel;     /** Datasets list model */
};
