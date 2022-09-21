#include "DatasetPickerAction.h"
#include "Application.h"
#include "DataHierarchyItem.h"

#include "event/Event.h"

#include <QHBoxLayout>

using namespace hdps;

DatasetPickerAction::DatasetPickerAction(QObject* parent, const QString& title, bool allDatasets /*= false*/, hdps::Datasets datasets /*= hdps::Datasets()*/, hdps::Dataset<hdps::DatasetImpl> currentDataset /*= hdps::Dataset<hdps::DatasetImpl>()*/) :
    OptionAction(parent, "Pick dataset"),
    _allDatasets(allDatasets),
    _datasetsModel(),
    _eventListener()
{
    setText(title);
    setIcon(Application::getIconFont("FontAwesome").getIcon("database"));
    setToolTip("Pick a dataset");
    setCustomModel(&_datasetsModel);
    setPlaceHolderString("--choose dataset--");

    connect(this, &OptionAction::currentIndexChanged, this, [this](const std::int32_t& currentIndex) {
        emit datasetPicked(_datasetsModel.getDataset(currentIndex));
    });

    if (_allDatasets) {
        _eventListener.setEventCore(Application::core());
        _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataAdded));
        _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataAboutToBeRemoved));
        _eventListener.registerDataEvent([this](DataEvent* dataEvent) {
            switch (dataEvent->getType()) {
            case EventType::DataAdded:
            case EventType::DataAboutToBeRemoved:
                fetchAllDatasetsFromCore();
                break;
            }
            });

        fetchAllDatasetsFromCore();
    }
    else {
        setDatasets(datasets);
    }

    setCurrentDataset(currentDataset);
}

QString DatasetPickerAction::getTypeString() const
{
    return "Dataset";
}

void DatasetPickerAction::setDatasets(Datasets datasets)
{
    _datasetsModel.setDatasets(datasets);

    for (auto& dataset : _datasetsModel.getDatasets()) {

        connect(&dataset, &Dataset<DatasetImpl>::dataAboutToBeRemoved, this, [this, dataset]() {
            _datasetsModel.removeDataset(dataset);
        });

        connect(&dataset, &Dataset<DatasetImpl>::dataGuiNameChanged, &_datasetsModel, &DatasetsModel::updateData);
    }

    auto publicDatasetPickerAction = dynamic_cast<DatasetPickerAction*>(_publicAction);

    if (publicDatasetPickerAction)
        setCurrentDataset(publicDatasetPickerAction->getCurrentDataset());
}

Dataset<DatasetImpl> DatasetPickerAction::getCurrentDataset() const
{
    return _datasetsModel.getDataset(getCurrentIndex());
}

void DatasetPickerAction::setCurrentDataset(Dataset<DatasetImpl> currentDataset)
{
    const auto currentIndex = _datasetsModel.rowIndex(currentDataset);

    if (currentIndex < 0)
        return;

    setCurrentIndex(currentIndex);
}

void DatasetPickerAction::setCurrentDataset(const QString& guid)
{
    const auto matches = _datasetsModel.match(QModelIndex(), Qt::EditRole, guid, 1);

    if (matches.isEmpty())
        return;

    setCurrentIndex(matches.first().row());
}

QString DatasetPickerAction::getCurrentDatasetGuid() const
{
    return getCurrentDataset().getDatasetGuid();
}

void DatasetPickerAction::fetchAllDatasetsFromCore()
{
    setDatasets(Application::core()->requestAllDataSets());
}

bool DatasetPickerAction::mayPublish() const
{
    return true;
}

void DatasetPickerAction::connectToPublicAction(WidgetAction* publicAction)
{
    auto publicDatasetPickerAction = dynamic_cast<DatasetPickerAction*>(publicAction);

    Q_ASSERT(publicDatasetPickerAction != nullptr);

    connect(this, &DatasetPickerAction::datasetPicked, publicDatasetPickerAction, qOverload<hdps::Dataset<hdps::DatasetImpl>>(&DatasetPickerAction::setCurrentDataset));
    connect(publicDatasetPickerAction, &DatasetPickerAction::datasetPicked, this, qOverload<hdps::Dataset<hdps::DatasetImpl>>(&DatasetPickerAction::setCurrentDataset));

    setCurrentDataset(publicDatasetPickerAction->getCurrentDataset());

    WidgetAction::connectToPublicAction(publicAction);
}

void DatasetPickerAction::disconnectFromPublicAction()
{
    auto publicDatasetPickerAction = dynamic_cast<DatasetPickerAction*>(_publicAction);

    Q_ASSERT(publicDatasetPickerAction != nullptr);

    disconnect(this, &DatasetPickerAction::datasetPicked, publicDatasetPickerAction, qOverload<hdps::Dataset<hdps::DatasetImpl>>(&DatasetPickerAction::setCurrentDataset));
    disconnect(publicDatasetPickerAction, &DatasetPickerAction::datasetPicked, this, qOverload<hdps::Dataset<hdps::DatasetImpl>>(&DatasetPickerAction::setCurrentDataset));

    WidgetAction::disconnectFromPublicAction();
}

hdps::gui::WidgetAction* DatasetPickerAction::getPublicCopy() const
{
    return new DatasetPickerAction(parent(), text(), true, _datasetsModel.getDatasets(), getCurrentDataset());
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
    return 2;
}

QVariant DatasetPickerAction::DatasetsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const auto column   = static_cast<Column>(index.column());
    const auto dataset  = _datasets.at(index.row());

    switch (role)
    {
        case Qt::DecorationRole:
            return dataset->getIcon();

        case Qt::DisplayRole:
        {
            switch (column)
            {
                case Column::Name:
                    return _showFullPathName ? dataset->getDataHierarchyItem().getFullPathName() : dataset->getGuiName();

                case Column::GUID:
                    return dataset->getGuid();

                default:
                    break;
            }
        }

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

    for (const auto& dataset : datasets)
        addDataset(dataset);
}

void DatasetPickerAction::DatasetsModel::addDataset(const Dataset<DatasetImpl>& dataset)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    {
        _datasets << dataset;
    }
    endInsertRows();

    auto& addedDataset = _datasets.last();

    connect(&addedDataset, &Dataset<DatasetImpl>::dataAboutToBeRemoved, this, [this, &addedDataset]() {
        removeDataset(addedDataset);
    });

    connect(&addedDataset, &Dataset<DatasetImpl>::dataGuiNameChanged, this, [this, &addedDataset]() {
        const auto colorDatasetRowIndex = rowIndex(addedDataset);

        if (colorDatasetRowIndex < 0)
            return;

        const auto modelIndex = index(colorDatasetRowIndex, 0);

        if (!modelIndex.isValid())
            return;

        emit dataChanged(modelIndex, modelIndex);
    });
}

void DatasetPickerAction::DatasetsModel::removeDataset(const hdps::Dataset<hdps::DatasetImpl>& dataset)
{
    const auto datasetRowIndex = rowIndex(dataset);

    beginRemoveRows(QModelIndex(), datasetRowIndex, datasetRowIndex);
    {
        _datasets.removeOne(dataset);
    }
    endRemoveRows();
}

void DatasetPickerAction::DatasetsModel::removeAllDatasets()
{
    if (rowCount() == 0)
        return;

    beginRemoveRows(QModelIndex(), 0, std::max(0, rowCount() - 1));
    {
        _datasets.clear();
    }
    endRemoveRows();

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
    for (auto dataset : _datasets) {

        if (!dataset.isValid())
            continue;

        const auto datasetModelIndex = index(_datasets.indexOf(dataset), 0);

        emit dataChanged(datasetModelIndex, datasetModelIndex);
    }
}
