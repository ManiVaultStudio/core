#include "DatasetPickerAction.h"
#include "Application.h"
#include "DataHierarchyItem.h"

#include "event/Event.h"

#include <QHBoxLayout>

using namespace hdps;

DatasetPickerAction::DatasetPickerAction(QObject* parent, const QString& title) :
    OptionAction(parent, "Pick dataset"),
    _datasetsModel()
{
    setText(title);
    setIcon(Application::getIconFont("FontAwesome").getIcon("database"));
    setToolTip("Pick a dataset");
    setCustomModel(&_datasetsModel);
    setPlaceHolderString("--choose dataset--");

    // Inform others that a dataset is picked when the dataset selection action current index changes
    connect(this, &OptionAction::currentIndexChanged, this, [this](const std::int32_t& currentIndex) {
        emit datasetPicked(_datasetsModel.getDataset(currentIndex));
    });
}

void DatasetPickerAction::setDatasets(const QVector<Dataset<DatasetImpl>>& datasets)
{
    // Set model datasets
    _datasetsModel.setDatasets(datasets);

    // Create connections
    for (auto& dataset : _datasetsModel.getDatasets()) {

        // Update the datasets when the dataset is removed
        connect(&dataset, &Dataset<DatasetImpl>::dataAboutToBeRemoved, this, [this, dataset]() {

            // Remove the dataset from the model
            _datasetsModel.removeDataset(dataset);
        });

        // Update the datasets model when the dataset is renamed
        connect(&dataset, &Dataset<DatasetImpl>::dataGuiNameChanged, &_datasetsModel, &DatasetsModel::updateData);
    }
}

Dataset<DatasetImpl> DatasetPickerAction::getCurrentDataset() const
{
    return _datasetsModel.getDataset(getCurrentIndex());
}

void DatasetPickerAction::setCurrentDataset(const Dataset<DatasetImpl>& currentDataset)
{
    // Get index of current dataset
    const auto currentIndex = _datasetsModel.rowIndex(currentDataset);

    // Only proceed if dataset was found
    if (currentIndex < 0)
        return;

    // Change current index
    setCurrentIndex(currentIndex);
}

DatasetPickerAction::DatasetsModel::DatasetsModel(QObject* parent /*= nullptr*/) :
    QAbstractListModel(parent),
    _datasets(),
    _showFullPathName(true)
{
}

int DatasetPickerAction::DatasetsModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    return _datasets.count();
}

int DatasetPickerAction::DatasetsModel::rowIndex(const Dataset<DatasetImpl>& dataset) const
{
    return _datasets.indexOf(dataset);
}

int DatasetPickerAction::DatasetsModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    return 1;
}

QVariant DatasetPickerAction::DatasetsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const auto column   = index.column();
    const auto dataset  = _datasets.at(index.row());

    switch (role)
    {
        case Qt::DecorationRole:
            return dataset->getIcon();

        case Qt::DisplayRole:
            return _showFullPathName ? dataset->getDataHierarchyItem().getFullPathName() : dataset->getGuiName();

        default:
            break;
    }

    return QVariant();
}

const QVector<Dataset<DatasetImpl>>& DatasetPickerAction::DatasetsModel::getDatasets() const
{
    return _datasets;
}

Dataset<DatasetImpl> DatasetPickerAction::DatasetsModel::getDataset(const std::int32_t& rowIndex) const
{
    if (rowIndex < 0)
        return Dataset<DatasetImpl>();

    return _datasets[rowIndex];
}

void DatasetPickerAction::DatasetsModel::setDatasets(const QVector<Dataset<DatasetImpl>>& datasets)
{
    removeAllDatasets();

    // Add datasets one-by-one
    for (const auto& dataset : datasets)
        addDataset(dataset);
}

void DatasetPickerAction::DatasetsModel::addDataset(const Dataset<DatasetImpl>& dataset)
{
    // Insert row into model
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    {
        // Add the dataset
        _datasets << dataset;
    }
    endInsertRows();

    // Get smart pointer to last added dataset
    auto& addedDataset = _datasets.last();

    // Remove a dataset from the model when it is about to be deleted
    connect(&addedDataset, &Dataset<DatasetImpl>::dataAboutToBeRemoved, this, [this, &addedDataset]() {
        removeDataset(addedDataset);
    });

    // Notify others that the model has updated when the dataset GUI name changes
    connect(&addedDataset, &Dataset<DatasetImpl>::dataGuiNameChanged, this, [this, &addedDataset]() {

        // Get row index of the dataset
        const auto colorDatasetRowIndex = rowIndex(addedDataset);

        // Only proceed if we found a valid row index
        if (colorDatasetRowIndex < 0)
            return;

        // Establish model index
        const auto modelIndex = index(colorDatasetRowIndex, 0);

        // Only proceed if we have a valid model index
        if (!modelIndex.isValid())
            return;

        // Notify others that the data changed
        emit dataChanged(modelIndex, modelIndex);
    });
}

void DatasetPickerAction::DatasetsModel::removeDataset(const hdps::Dataset<hdps::DatasetImpl>& dataset)
{
    // Get row index of the dataset
    const auto datasetRowIndex = rowIndex(dataset);

    // Remove row from model
    beginRemoveRows(QModelIndex(), datasetRowIndex, datasetRowIndex);
    {
        // Remove dataset from internal vector
        _datasets.removeOne(dataset);
    }
    endRemoveRows();
}

void DatasetPickerAction::DatasetsModel::removeAllDatasets()
{
    // No point in removing zero rows
    if (rowCount() == 0)
        return;

    // Remove row from model
    beginRemoveRows(QModelIndex(), 0, std::max(0, rowCount() - 1));
    {
        // Remove all datasets
        _datasets.clear();
    }
    endRemoveRows();

    // And update model data with altered datasets
    updateData();
}

bool DatasetPickerAction::DatasetsModel::getShowFullPathName() const
{
    return _showFullPathName;
}

void DatasetPickerAction::DatasetsModel::setShowFullPathName(const bool& showFullPathName)
{
    _showFullPathName = showFullPathName;

    updateData();
}

void DatasetPickerAction::DatasetsModel::updateData()
{
    // Update the datasets string list model
    for (auto dataset : _datasets) {

        // Continue if the dataset is not valid
        if (!dataset.isValid())
            continue;

        // Get dataset model index
        const auto datasetModelIndex = index(_datasets.indexOf(dataset), 0);

        // Notify others that the data changed
        emit dataChanged(datasetModelIndex, datasetModelIndex);
    }
}
