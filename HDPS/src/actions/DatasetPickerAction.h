#pragma once

#include "Dataset.h"

#include "actions/WidgetAction.h"
#include "actions/OptionAction.h"

#include "Set.h"

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

public:

    /** 
     * Constructor
     * @param parent Pointer to parent object
     */
    DatasetPickerAction(QObject* parent);

    /**
     * Set the datasets from which can be picked
     * @param datasets Datasets from which can be picked
     */
    void setDatasets(const QVector<hdps::Dataset<hdps::DatasetImpl>>& datasets);

    /** Get the current dataset */
    hdps::Dataset<hdps::DatasetImpl> getCurrentDataset() const;

    /**
     * Set the current dataset
     * @param currentDataset Current dataset reference
     */
    void setCurrentDataset(const hdps::Dataset<hdps::DatasetImpl>& currentDataset);

    /** Get whether to show the full path name in the GUI */
    bool getShowFullPathName() const;

    /**
     * Set whether to show the full path name in the GUI
     * @param showFullPathName Whether to show the full path name in the GUI
     */
    void setShowFullPathName(const bool& showFullPathName);

protected:

    /** Update the current dataset action from the dataset */
    void updateCurrentDatasetAction();

signals:

    /**
     * Signals that a dataset has been picked
     * @param Dataset reference to picked dataset
     */
    void datasetPicked(const hdps::Dataset<hdps::DatasetImpl>& pickedDataset);

protected:
    QVector<hdps::Dataset<hdps::DatasetImpl>>   _datasets;                  /** Datasets from which can be picked */
    bool                                        _showFullPathName;          /** Whether to show the full path name in the GUI */
};
