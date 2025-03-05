// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractDatasetsModel.h"

#include "Application.h"
#include "CoreInterface.h"
#include "Set.h"

#include <util/Exception.h>

#include <actions/WidgetAction.h>

#ifdef _DEBUG
    #define ABSTRACT_DATASETS_MODEL_VERBOSE
#endif

namespace mv
{

using namespace util;
using namespace gui;

AbstractDatasetsModel::HeaderItem::HeaderItem(const ColumHeaderInfo& columHeaderInfo) :
    QStandardItem(),
    _columHeaderInfo(columHeaderInfo)
{
}

QVariant AbstractDatasetsModel::HeaderItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role)
    {
        case Qt::DisplayRole:
            return _columHeaderInfo._display;

        case Qt::EditRole:
            return _columHeaderInfo._edit;

        case Qt::ToolTipRole:
            return _columHeaderInfo._tooltip;

        default:
            break;
    }

    return QVariant();
}

AbstractDatasetsModel::Item::Item(AbstractDatasetsModel& datasetsModel, Dataset<DatasetImpl> dataset, bool editable /*= false*/) :
    QStandardItem(),
    QObject(),
    _datasetsModel(datasetsModel),
    _dataset(dataset)
{
    Q_ASSERT(_dataset.isValid());

    setEditable(editable);
    setDropEnabled(true);
}

AbstractDatasetsModel& AbstractDatasetsModel::Item::getDatasetsModel()
{
    return _datasetsModel;
}

Dataset<DatasetImpl>& AbstractDatasetsModel::Item::getDataset()
{
    return _dataset;
}

AbstractDatasetsModel::NameItem::NameItem(AbstractDatasetsModel& datasetsModel, Dataset<DatasetImpl> dataset) :
    Item(datasetsModel, dataset)
{
    connect(&datasetsModel.getShowIconAction(), &ToggleAction::toggled, this, [this](bool toggled) -> void {
        emitDataChanged();
    });

    connect(getDataset().get(), &DatasetImpl::textChanged, this, [this](const QString& name) -> void {
        emitDataChanged();
    });
}

QVariant AbstractDatasetsModel::NameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    auto nonConstThis = const_cast<AbstractDatasetsModel::NameItem*>(this);

    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return nonConstThis->getDataset()->getGuiName();

        case Qt::ToolTipRole:
            return "Dataset name: " + data(Qt::DisplayRole).toString();

        case Qt::DecorationRole:
            return nonConstThis->getDatasetsModel().getShowIconAction().isChecked() ? StyledIcon(nonConstThis->getDataset()->icon()) : QIcon();

        default:
            break;
    }

    return Item::data(role);
}

AbstractDatasetsModel::LocationItem::LocationItem(AbstractDatasetsModel& datasetsModel, Dataset<DatasetImpl> dataset) :
    Item(datasetsModel, dataset)
{
    connect(getDataset().get(), &DatasetImpl::locationChanged, this, [this](const QString& location) -> void {
        emitDataChanged();
    });
}

QVariant AbstractDatasetsModel::LocationItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return const_cast<AbstractDatasetsModel::LocationItem*>(this)->getDataset()->getLocation();

        case Qt::ToolTipRole:
            return "Dataset location: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

AbstractDatasetsModel::IdItem::IdItem(AbstractDatasetsModel& datasetsModel, Dataset<DatasetImpl> dataset) :
    Item(datasetsModel, dataset)
{
    connect(getDataset().get(), &DatasetImpl::idChanged, this, [this](const QString& id) -> void {
        emitDataChanged();
    });
}

QVariant AbstractDatasetsModel::IdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return const_cast<AbstractDatasetsModel::IdItem*>(this)->getDataset()->getId();

        case Qt::ToolTipRole:
            return "Dataset globally unique identifier: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractDatasetsModel::RawDataNameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return const_cast<AbstractDatasetsModel::RawDataNameItem*>(this)->getDataset()->getRawDataName();

        case Qt::ToolTipRole:
            return "Raw data name: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractDatasetsModel::SourceDatasetIdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return const_cast<AbstractDatasetsModel::SourceDatasetIdItem*>(this)->getDataset()->getSourceDataset<DatasetImpl>()->getId();

        case Qt::ToolTipRole:
            return "Source dataset ID: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QMap<AbstractDatasetsModel::Column, AbstractDatasetsModel::ColumHeaderInfo> AbstractDatasetsModel::columnInfo = QMap<AbstractDatasetsModel::Column, AbstractDatasetsModel::ColumHeaderInfo>({
    { AbstractDatasetsModel::Column::Name, { "Name" , "Name", "Name of the dataset" } },
    { AbstractDatasetsModel::Column::Location, { "" , "Enabled", "Whether the dataset is enabled or not" } },
    { AbstractDatasetsModel::Column::ID, { "ID",  "ID", "Globally unique identifier of the dataset" } },
    { AbstractDatasetsModel::Column::RawDataName, { "Raw data name",  "Raw data name", "Name of the raw data" } },
    { AbstractDatasetsModel::Column::SourceDatasetID, { "Source dataset ID",  "Source dataset ID", "Globally unique identifier of the source dataset" } }
});

AbstractDatasetsModel::AbstractDatasetsModel(PopulationMode populationMode /*= PopulationMode::Automatic*/, QObject* parent /*= nullptr*/) :
    StandardItemModel(parent),
    _populationMode(),
    _showIconAction(this, "Show icon", true)
{
    setColumnCount(static_cast<int>(Column::Count));

    for (auto column : columnInfo.keys())
        setHorizontalHeaderItem(static_cast<int>(column), new HeaderItem(columnInfo[column]));

    setPopulationMode(populationMode);
}

QModelIndex AbstractDatasetsModel::getIndexFromDataset(Dataset<DatasetImpl> dataset) const
{
    try {
        if (!dataset.isValid())
            throw std::runtime_error("Dataset is not valid");

        return getItemFromDataset(dataset)->index();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get index from dataset ID in datasets model", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to get index from dataset ID in datasets model");
    }

    return {};
}

QModelIndex AbstractDatasetsModel::getIndexFromDataset(const QString& datasetId) const
{
    try {
        return getIndexFromDataset(mv::data().getDataset(datasetId));
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get index from dataset ID in datasets model", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to get index from dataset ID in datasets model");
    }

    return {};
}

QStandardItem* AbstractDatasetsModel::getItemFromDataset(const QString& datasetId) const
{
    const auto matches = match(index(0, static_cast<int>(Column::ID)), Qt::EditRole, datasetId, 1, Qt::MatchExactly | Qt::MatchRecursive);

    if (matches.isEmpty())
        throw std::runtime_error(QString("Dataset with id %1 not found").arg(datasetId).toStdString());

    return itemFromIndex(matches.first().siblingAtColumn(static_cast<int>(Column::Name)));
}

QStandardItem* AbstractDatasetsModel::getItemFromDataset(Dataset<DatasetImpl> dataset) const
{
    if (!dataset.isValid())
        throw std::runtime_error("Supplied dataset is not valid");

    const auto matches = match(index(0, static_cast<int>(Column::ID)), Qt::EditRole, dataset->getId(), 1, Qt::MatchExactly | Qt::MatchRecursive);

    if (matches.isEmpty())
        throw std::runtime_error(QString("%1 (%2) not found").arg(dataset->getGuiName(), dataset->getId()).toStdString());

    return itemFromIndex(matches.first().siblingAtColumn(static_cast<int>(Column::Name)));
}

AbstractDatasetsModel::PopulationMode AbstractDatasetsModel::getPopulationMode() const
{
    return _populationMode;
}

void AbstractDatasetsModel::setPopulationMode(PopulationMode populationMode)
{
#ifdef ABSTRACT_DATASETS_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _populationMode = populationMode;

    switch (_populationMode)
    {
        case PopulationMode::Manual: {
            disconnect(&mv::data(), &AbstractDataManager::datasetAdded, this, nullptr);

            break;
        }

        case PopulationMode::Automatic: {
            connect(&mv::data(), &AbstractDataManager::datasetAdded, this, [this](Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, bool visible = true) -> void {
                addDataset(dataset);
            });

            connect(&mv::data(), &AbstractDataManager::datasetAboutToBeRemoved, this, [this](Dataset<DatasetImpl> dataset) {
                removeDataset(dataset);
            });

            break;
        }

        default:
            break;
    }
}

Datasets AbstractDatasetsModel::getDatasets() const
{
    return _datasets;
}

Dataset<DatasetImpl> AbstractDatasetsModel::getDataset(std::int32_t rowIndex) const
{
    auto nameItem = dynamic_cast<Item*>(itemFromIndex(index(rowIndex, static_cast<int>(Column::Name))));

    if (!nameItem)
        return {};

    return nameItem->getDataset();
}

void AbstractDatasetsModel::setDatasets(mv::Datasets datasets)
{
    for (const auto& dataset : _datasets)
        removeDataset(dataset);

    for (const auto& dataset : datasets)
        addDataset(dataset);
}

void AbstractDatasetsModel::addDataset(Dataset<DatasetImpl> dataset)
{
    _datasets << dataset;
}

void AbstractDatasetsModel::removeDataset(Dataset<DatasetImpl> dataset)
{
    try {
        auto datasetItem = getItemFromDataset(dataset);

#ifdef ABSTRACT_DATASETS_MODEL_VERBOSE
        qDebug() << __FUNCTION__ << dataset->getLocation();
#endif

        if (!removeRow(datasetItem->row()))
            throw std::runtime_error("Remove row failed");

        if (!_datasets.contains(dataset))
            throw std::runtime_error("Dataset no found");

        _datasets.removeOne(dataset);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove dataset from datasets model", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to remove dataset from datasets model");
    }
}

}
