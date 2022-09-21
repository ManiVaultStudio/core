#pragma once

#include "Dataset.h"
#include "Set.h"

#include "actions/WidgetAction.h"
#include "actions/OptionAction.h"
#include "event/EventListener.h"

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
class DatasetPickerAction : public OptionAction
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

        /** Model columns */
        enum class Column {
            Name,
            GUID
        };

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
         * Add dataset
         * @param dataset Smart pointer to dataset
         */
        void addDataset(const hdps::Dataset<hdps::DatasetImpl>& dataset);

        /**
         * Remove specific dataset
         * @param dataset Smart pointer to dataset
         */
        void removeDataset(const hdps::Dataset<hdps::DatasetImpl>& dataset);

        /** Remove all datasets from the model */
        void removeAllDatasets();

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
     * @param allDatasets Select from all datasets in the data hierarchy
     * @param datasets Selectable datasets
     * @param currentDataset Current dataset
     */
    DatasetPickerAction(QObject* parent, const QString& title, bool allDatasets = false, hdps::Datasets datasets = hdps::Datasets(), hdps::Dataset<hdps::DatasetImpl> currentDataset = hdps::Dataset<hdps::DatasetImpl>());

    /**
     * Get type string
     * @return Widget action type in string format
     */
    QString getTypeString() const override;

    /** Get whether to synchronizes the selectable datasets with all core datasets */
    bool getAllDatasetsFromCore() const;

    /**
     * Set the datasets from which can be picked
     * @param datasets Datasets from which can be picked
     */
    void setDatasets(Datasets datasets);

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
     * @param currentDataset Smart pointer to current dataset
     */
    void setCurrentDataset(hdps::Dataset<hdps::DatasetImpl> currentDataset);

    /**
     * Set the current dataset by \p guid
     * @param guid Current dataset globally unique identifier
     */
    void setCurrentDataset(const QString& guid);

    /**
     * Get current dataset globally unique identifier
     * @return The globally unique identifier of the currently selected dataset (if any)
     */
    QString getCurrentDatasetGuid() const;

private:

    /** Updates the datasets with all from the core */
    void fetchAllDatasetsFromCore();

public: // Linking

    /**
     * Get whether the action may be published or not
     * @return Boolean indicating whether the action may be published or not
     */
    bool mayPublish() const override;

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     */
    void connectToPublicAction(WidgetAction* publicAction) override;

    /** Disconnect this action from a public action */
    void disconnectFromPublicAction() override;

protected:  // Linking

    /**
     * Get public copy of the action (other compatible actions can connect to it)
     * @return Pointer to public copy of the action
     */
    virtual WidgetAction* getPublicCopy() const override;

signals:

    /**
     * Signals that a dataset has been picked
     * @param Smart pointer to picked dataset
     */
    void datasetPicked(hdps::Dataset<hdps::DatasetImpl> pickedDataset);

    /**
     * Signals that selectable datasets changed
     * @param Selectable datasets
     */
    void datasetsChanged(hdps::Datasets datasets);

protected:
    bool            _allDatasets;       /** Select from all datasets in the data hierarchy */
    DatasetsModel   _datasetsModel;     /** Datasets list model */
    EventListener   _eventListener;     /** Listen to events from the core */
};
