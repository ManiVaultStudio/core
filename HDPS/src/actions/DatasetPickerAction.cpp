#include "DatasetPickerAction.h"
#include "Application.h"
#include "DataHierarchyItem.h"

#include "event/Event.h"

#include <QHBoxLayout>

using namespace hdps;

DatasetPickerAction::DatasetPickerAction(QObject* parent) :
    OptionAction(parent, "Pick dataset"),
    _datasets(),
    _showFullPathName()
{
    setText("Dataset picker");
    setIcon(Application::getIconFont("FontAwesome").getIcon("database"));
    setToolTip("Pick a dataset");

    connect(this, &OptionAction::currentIndexChanged, this, [this](const std::int32_t& currentIndex) {
        emit datasetPicked(_datasets[currentIndex].get());
    });
}

void DatasetPickerAction::setDatasets(const QVector<Dataset<DatasetImpl>>& datasets)
{
    // Assign datasets
    _datasets = datasets;

    // Create connections
    for (auto& dataset : _datasets) {

        // Update the datasets when the dataset is removed
        connect(&dataset, &Dataset<DatasetImpl>::dataAboutToBeRemoved, this, [this, dataset]() {

            // Remove the dataset from the slit
            _datasets.removeOne(dataset);

            // And update the dataset selection picker action
            updateCurrentDatasetAction();
        });

        // Update the current dataset action when the dataset is renamed
        connect(&dataset, &Dataset<DatasetImpl>::dataGuiNameChanged, this, &DatasetPickerAction::updateCurrentDatasetAction);
    }

    // And update the options action
    updateCurrentDatasetAction();
}

Dataset<DatasetImpl> DatasetPickerAction::getCurrentDataset() const
{
    // Get the current dataset index
    const auto currentIndex = getCurrentIndex();

    if (currentIndex >= 0)
        return _datasets[currentIndex];

    return Dataset<DatasetImpl>();
}

void DatasetPickerAction::setCurrentDataset(const Dataset<DatasetImpl>& currentDataset)
{
    // Get index of current dataset
    const auto currentIndex = _datasets.indexOf(currentDataset);

    // Only proceed if dataset was found
    if (currentIndex < 0)
        return;

    // Change current index
    setCurrentIndex(currentIndex);
}

bool DatasetPickerAction::getShowFullPathName() const
{
    return _showFullPathName;
}

void DatasetPickerAction::setShowFullPathName(const bool& showFullPathName)
{
    _showFullPathName = showFullPathName;

    updateCurrentDatasetAction();
}

void DatasetPickerAction::updateCurrentDatasetAction()
{
    // Dataset options
    QStringList datasets;

    // Add option for each dataset
    for (auto dataset : _datasets)
        datasets << (_showFullPathName ? dataset->getDataHierarchyItem().getFullPathName() : dataset->getGuiName());

    // Set options in picker action
    setOptions(datasets);
}
