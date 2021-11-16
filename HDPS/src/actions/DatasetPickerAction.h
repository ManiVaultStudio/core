#pragma once

#include "actions/WidgetAction.h"
#include "actions/OptionAction.h"
#include "util/DatasetRef.h"

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
class DatasetPickerAction : public WidgetAction
{
Q_OBJECT

public:

    /** Widget class for dataset picker action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param datasetPickerAction Pointer to dataset picker action
         */
        Widget(QWidget* parent, DatasetPickerAction* datasetPickerAction);

    protected:
        friend class DatasetPickerAction;
    };

protected:

    /**
     * Get widget representation of the points dimension action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

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
    void setDatasets(const QVector<DatasetRef<hdps::DataSet>>& datasets);

    /**
     * Set the current dataset
     * @param currentDataset Current dataset reference
     */
    void setCurrentDataset(const DatasetRef<hdps::DataSet>& currentDataset);

protected:

    /** Update the current dataset action from the dataset */
    void updateCurrentDatasetAction();

public: /** Action getters */

    OptionAction& getCurrentDatasetAction() { return _currentDatasetAction; }

signals:

    /**
     * Signals that a dataset has been picked
     * @param Dataset reference to picked dataset
     */
    void datasetPicked(const DatasetRef<hdps::DataSet>& pickedDataset);

protected:
    QVector<DatasetRef<hdps::DataSet>>  _datasets;                  /** Datasets from which can be picked */
    OptionAction                        _currentDatasetAction;      /** Pick dataset action */
};
