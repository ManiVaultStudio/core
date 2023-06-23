#include "DatasetPickerAction.h"
#include "Application.h"
#include "DataHierarchyItem.h"

#include "event/Event.h"

#include <QHBoxLayout>

#ifdef _DEBUG
    #define DATASET_PICKER_ACTION_VERBOSE
#endif

using namespace hdps::util;

namespace hdps::gui {

DatasetPickerAction::DatasetPickerAction(QObject* parent /*= nullptr*/, const QString& title /*= ""*/, Mode mode /*= Mode::Automatic*/) :
    OptionAction(parent, title),
    _mode(mode),
    _datasetsFilterFunction(),
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

    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetAdded));
    _eventListener.registerDataEvent([this](DataEvent* dataEvent) {
        switch (dataEvent->getType()) {
            case EventType::DatasetAdded:
                populateDatasetsFromCore();
                break;
        }
    });

    switch (_mode)
    {
        case DatasetPickerAction::Mode::Manual:
            break;

        case DatasetPickerAction::Mode::Automatic:
            populateDatasetsFromCore();
            break;
        
        default:
            break;
    }
}

DatasetPickerAction::Mode DatasetPickerAction::getMode() const
{
    return _mode;
}

void DatasetPickerAction::setMode(Mode mode)
{
#ifdef DATASET_PICKER_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (mode == _mode)
        return;

    _mode = mode;

    switch (_mode)
    {
        case DatasetPickerAction::Mode::Manual:
            break;

        case DatasetPickerAction::Mode::Automatic:
            populateDatasetsFromCore();
            break;

        default:
            break;
    }
}

void DatasetPickerAction::setDatasets(Datasets datasets)
{
#ifdef DATASET_PICKER_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _mode = Mode::Manual;

    _datasetsModel.setDatasets(datasets);

    for (auto& dataset : _datasetsModel.getDatasets()) {

        connect(&dataset, &Dataset<DatasetImpl>::datasetAboutToBeRemoved, this, [this, dataset]() {
            _datasetsModel.removeDataset(dataset);
        });

        connect(dataset.get(), &DatasetImpl::textChanged, &_datasetsModel, &DatasetsModel::updateData);
    }

    auto publicDatasetPickerAction = dynamic_cast<DatasetPickerAction*>(getPublicAction());

    if (publicDatasetPickerAction)
        setCurrentDataset(publicDatasetPickerAction->getCurrentDataset());
}

void DatasetPickerAction::setDatasetsFilterFunction(const DatasetsFilterFunction& datasetsFilterFunction)
{
#ifdef DATASET_PICKER_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _mode                   = Mode::Automatic;
    _datasetsFilterFunction = datasetsFilterFunction;

    populateDatasetsFromCore();
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
    if (guid.isEmpty())
        return;

    const auto matches = _datasetsModel.match(_datasetsModel.index(0, static_cast<int>(DatasetsModel::Column::GUID)), Qt::EditRole, guid, -1);

    if (matches.isEmpty())
        return;

    setCurrentIndex(matches.first().row());
}

QString DatasetPickerAction::getCurrentDatasetGuid() const
{
    return getCurrentDataset().getDatasetId();
}

void DatasetPickerAction::populateDatasetsFromCore()
{
    if (_mode == Mode::Manual)
        return;

#ifdef DATASET_PICKER_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    auto datasets = Application::core()->requestAllDataSets();

    if (_datasetsFilterFunction)
        datasets = _datasetsFilterFunction(datasets);

    _datasetsModel.setDatasets(datasets);

    for (auto& dataset : datasets)
        connect(dataset.get(), &DatasetImpl::textChanged, &_datasetsModel, &DatasetsModel::updateData);

    auto publicDatasetPickerAction = dynamic_cast<DatasetPickerAction*>(getPublicAction());

    if (publicDatasetPickerAction)
        setCurrentDataset(publicDatasetPickerAction->getCurrentDataset());
}

void DatasetPickerAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicDatasetPickerAction = dynamic_cast<DatasetPickerAction*>(publicAction);

    Q_ASSERT(publicDatasetPickerAction != nullptr);

    if (publicDatasetPickerAction == nullptr)
        return;

    connect(this, &DatasetPickerAction::datasetPicked, publicDatasetPickerAction, qOverload<hdps::Dataset<hdps::DatasetImpl>>(&DatasetPickerAction::setCurrentDataset));
    connect(publicDatasetPickerAction, &DatasetPickerAction::datasetPicked, this, qOverload<hdps::Dataset<hdps::DatasetImpl>>(&DatasetPickerAction::setCurrentDataset));

    setCurrentDataset(publicDatasetPickerAction->getCurrentDataset());

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void DatasetPickerAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicDatasetPickerAction = dynamic_cast<DatasetPickerAction*>(getPublicAction());

    Q_ASSERT(publicDatasetPickerAction != nullptr);

    if (publicDatasetPickerAction == nullptr)
        return;

    disconnect(this, &DatasetPickerAction::datasetPicked, publicDatasetPickerAction, qOverload<hdps::Dataset<hdps::DatasetImpl>>(&DatasetPickerAction::setCurrentDataset));
    disconnect(publicDatasetPickerAction, &DatasetPickerAction::datasetPicked, this, qOverload<hdps::Dataset<hdps::DatasetImpl>>(&DatasetPickerAction::setCurrentDataset));

    WidgetAction::disconnectFromPublicAction(recursive);
}

void DatasetPickerAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Value");

    setCurrentDataset(variantMap["Value"].toString());
}

QVariantMap DatasetPickerAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    variantMap.insert({
        { "Value", getCurrentDatasetGuid() }
    });

    return variantMap;
}

DatasetPickerAction::DatasetsModel::DatasetsModel(QObject* parent /*= nullptr*/) :
    QAbstractListModel(parent),
    _datasets(),
    _showLocation(true),
    _showIcon(true)
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

    if (!dataset.isValid())
        return QVariant();

    switch (role)
    {
        case Qt::DecorationRole:
            return _showIcon ? dataset->getIcon() : QVariant();

        case Qt::EditRole:
        case Qt::DisplayRole:
        {
            switch (column)
            {
                case Column::Name:
                    return _showLocation ? dataset->getLocation() : dataset->text();

                case Column::GUID:
                    return dataset->getId();

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

    connect(&addedDataset, &Dataset<DatasetImpl>::datasetAboutToBeRemoved, this, [this, &addedDataset]() {
        removeDataset(addedDataset);
    });

    connect(addedDataset.get(), &DatasetImpl::textChanged, this, [this, &addedDataset]() {
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

bool DatasetPickerAction::DatasetsModel::getShowIcon() const
{
    return _showIcon;
}

void DatasetPickerAction::DatasetsModel::setShowIcon(bool showIcon)
{
    if (showIcon == _showIcon)
        return;

    _showIcon = showIcon;

    updateData();
}

bool DatasetPickerAction::DatasetsModel::getShowLocation() const
{
    return _showLocation;
}

void DatasetPickerAction::DatasetsModel::setShowLocation(bool showLocation)
{
    if (showLocation == _showLocation)
        return;

    _showLocation = showLocation;

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

}
