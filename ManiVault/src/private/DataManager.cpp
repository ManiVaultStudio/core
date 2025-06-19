// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataManager.h"

#include "RemoveDatasetsDialog.h"
#include "GroupDataDialog.h"

#include <util/Exception.h>

#include <models/DatasetsListModel.h>

#include <ModalTask.h>
#include <RawData.h>
#include <DataType.h>
#include <DataHierarchyItem.h>
#include <AnalysisPlugin.h>

#include <stdexcept>

#ifdef _DEBUG
    //#define DATA_MANAGER_VERBOSE
#endif

using namespace mv::util;

namespace mv
{

using namespace plugin;

DataManager::DataManager(QObject* parent) :
    AbstractDataManager(parent),
    _datasetsListModel(nullptr)
{
}

DataManager::~DataManager()
{
    reset();
}

void DataManager::initialize()
{
#ifdef DATA_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractDataManager::initialize();

    if (isInitialized())
        return;

    beginInitialization();
    endInitialization();

    _datasetsListModel = new DatasetsListModel(AbstractDatasetsModel::PopulationMode::Automatic, this);
}

void DataManager::reset()
{
#ifdef DATA_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
        DataHierarchyItems dataHierarchyItems;

        for (auto& dataset : _datasets)
            dataHierarchyItems << &dataset->getDataHierarchyItem();

        std::sort(dataHierarchyItems.begin(), dataHierarchyItems.end(), [](auto dataHierarchyItemA, auto dataHierarchyItemB) -> bool {
            return dataHierarchyItemA->getDepth() < dataHierarchyItemB->getDepth();
        });

        std::reverse(dataHierarchyItems.begin(), dataHierarchyItems.end());

        for (auto dataHierarchyItem : dataHierarchyItems)
            removeDataset(dataHierarchyItem->getDataset());
    }
    endReset();
}

void DataManager::addRawData(plugin::RawData* rawData)
{
    try
    {

#ifdef DATA_MANAGER_VERBOSE
        qDebug() << "Add raw data" << rawData->getName() << "to the the data manager";
#endif

        _rawDataMap.emplace(rawData->getName(), rawData);

        emit rawDataAdded(rawData);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add raw data to the data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to add raw data to the data manager");
    }
}

void DataManager::removeRawData(const QString& rawDataName)
{
    try
    {
#ifdef DATA_MANAGER_VERBOSE
        qDebug() << "Remove raw data" << rawDataName << "from the data manager";
#endif

        if (rawDataName.isEmpty())
            throw std::runtime_error("Invalid raw data name");

        removeSelection(rawDataName);

        const auto it = _rawDataMap.find(rawDataName);

        if (it == _rawDataMap.end())
            throw std::runtime_error(QString("Raw data with name %1 not found").arg(rawDataName).toStdString());

        emit rawDataAboutToBeRemoved(_rawDataMap[rawDataName]);
        {
            plugins().destroyPlugin(_rawDataMap[rawDataName]);

            _rawDataMap.erase(it);
        }
        emit rawDataRemoved(rawDataName);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove raw data from data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to remove raw data from data manager");
    }
}

plugin::RawData* DataManager::getRawData(const QString& rawDataName)
{
    try
    {
        if (rawDataName.isEmpty())
            throw std::runtime_error("Raw data name is invalid");

        if (_rawDataMap.find(rawDataName) == _rawDataMap.end())
            throw std::runtime_error(QString("%1 not found").arg(rawDataName).toStdString());

        return _rawDataMap[rawDataName];
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get raw data from data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get raw data from data manager");
    }

    return {};
}

QStringList DataManager::getRawDataNames() const
{
    QStringList rawDataNames;

    for (const auto& [rawDataName, rawData] : _rawDataMap)
        rawDataNames << rawDataName;

    return rawDataNames;
}

std::uint64_t DataManager::getRawDataSize(const QString& rawDataName) const
{
    try
    {
        if (rawDataName.isEmpty())
            throw std::runtime_error("Raw data name is invalid");

        if (_rawDataMap.find(rawDataName) == _rawDataMap.end())
            throw std::runtime_error(QString("%1 not found").arg(rawDataName).toStdString());

        return const_cast<DataManager*>(this)->_rawDataMap[rawDataName]->getRawDataSize();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to get raw data size for %1").arg(rawDataName), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to get raw data size for %1").arg(rawDataName));
    }

    return {};
}

QString DataManager::getRawDataType(const QString& rawDataName) const
{
    try
    {
        if (rawDataName.isEmpty())
            throw std::runtime_error("Raw data name is invalid");

        if (_rawDataMap.find(rawDataName) == _rawDataMap.end())
            throw std::runtime_error(QString("%1 not found").arg(rawDataName).toStdString());

        return _rawDataMap.at(rawDataName)->getDataType().getTypeString();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get raw data type from the data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get raw data type from the data manager");
    }

    return "Undefined";
}

Dataset<> DataManager::createDataset(const QString& kind, const QString& guiName, const Dataset<DatasetImpl>& parentDataset /*= Dataset<DatasetImpl>()*/, const QString& id /*= ""*/, bool notify /*= true*/)
{
#ifdef DATA_MANAGER_VERBOSE
    qDebug() << "Create dataset" << kind << guiName;
#endif

    const auto rawDataName  = plugins().requestPlugin(kind)->getName();
    const auto rawData      = getRawData(rawDataName);

    // Create an initial full set and an empty selection belonging to the raw data
    auto fullSet = rawData->createDataSet(id);
    auto selection = rawData->createDataSet();

    // Set the properties of the new sets
    fullSet->setText(guiName);
    fullSet->setAll(true);

    fullSet->getTask().setName(guiName);

    selection->getTask().setName(QString("%1_selection").arg(guiName));

    // Set pointer of full dataset to itself just to avoid having to be wary of this not being set
    fullSet->_fullDataset = fullSet;

    addDataset(fullSet, parentDataset, notify);
    addSelection(rawDataName, selection);

    fullSet->init();

    return fullSet;
}

Dataset<> DataManager::createDatasetWithoutSelection(const QString& kind, const QString& guiName, const Dataset<DatasetImpl>& parentDataset /*= Dataset<DatasetImpl>()*/, const QString& id /*= ""*/, bool notify /*= true*/)
{
#ifdef DATA_MANAGER_VERBOSE
    qDebug() << "Create dataset without selection" << kind << guiName;
#endif

    const auto rawDataName  = plugins().requestPlugin(kind)->getName();
    const auto rawData      = getRawData(rawDataName);

    auto fullSet = rawData->createDataSet(id);

    fullSet->setText(guiName);
    fullSet->setAll(true);

    fullSet->getTask().setName(guiName);

    fullSet->_fullDataset = fullSet;

    addDataset(fullSet, parentDataset, notify);

    fullSet->init();

    return fullSet;
}

void DataManager::addDataset(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, bool notify /*= true*/)
{
    try
    {

#ifdef DATA_MANAGER_VERBOSE
        qDebug() << "Add dataset" << dataset->getGuiName() << dataset->getId() << "to the data manager";
#endif

        if (!dataset.isValid())
            throw std::runtime_error("Dataset smart pointer is invalid");

        _datasets.push_back(std::unique_ptr<DatasetImpl>(dataset.get()));

        dataHierarchy().addItem(dataset, parentDataset);

        if (notify)
            events().notifyDatasetAdded(dataset);

        emit datasetAdded(dataset, parentDataset, true);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add dataset to the data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to add dataset to the data manager");
    }
}

void DataManager::removeDataset(Dataset<DatasetImpl> dataset)
{
    try {
#ifdef DATA_MANAGER_VERBOSE
    	qDebug() << "Remove dataset" << dataset->getGuiName() << "from the data manager";
#endif

    	if (!dataset.isValid())
    		throw std::runtime_error("Dataset smart pointer is invalid");

    	if (dataset->isLocked()) {

#ifdef DATA_MANAGER_VERBOSE
    		qDebug() << "Dataset is locked and will be removed as soon as it is un-locked";
#endif

    		connect(&dataset->getDataHierarchyItem(), &DataHierarchyItem::lockedChanged, this, [this, dataset](bool locked) -> void {
				disconnect(&dataset->getDataHierarchyItem(), &DataHierarchyItem::lockedChanged, this, nullptr);

				if (!locked)
					removeDataset(dataset);
				});
    	}

    	const auto datasetId = dataset->getId();
        const auto datasetDataType = dataset->getDataType();
    	const auto rawDataName = dataset->getRawDataName();

    	dataset->setAboutToBeRemoved();

    	for (const auto& underiveDataset : _datasets) {
    		if (underiveDataset->isDerivedData() && underiveDataset->getNextSourceDataset<DatasetImpl>()->getId() == dataset->getId()) {
    			if (underiveDataset->mayUnderive()) {
    				underiveDataset->_derived = false;
    				underiveDataset->setSourceDataset(Dataset<DatasetImpl>());
    			}
    			else {
    				removeDataset(underiveDataset.get());
    			}
    		}
    	}

        if (!mv::core()->isAboutToBeDestroyed()) {
	        for (const auto dataHierarchyItem : dataset->getDataHierarchyItem().getChildren()) {
	        	if (!dataHierarchyItem->getDataset()->mayUnderive())
	        		removeDataset(dataHierarchyItem->getDataset());
	        	else
	        		dataHierarchyItem->setParent(nullptr);
	        }
        }

    	dataset->setLocked(true);

    	if (!mv::core()->isAboutToBeDestroyed()) {
    		events().notifyDatasetAboutToBeRemoved(dataset);
    		emit datasetAboutToBeRemoved(dataset);
		}

        const auto it = std::find_if(_datasets.begin(), _datasets.end(), [datasetId](const auto& datasetPtr) -> bool {
            return datasetId == datasetPtr->getId();
        });
    
        if (it == _datasets.end())
            throw std::runtime_error(QString("Dataset with id %1 not found in database").arg(dataset->getId()).toStdString());
    
        if (auto analysisPlugin = dataset->getAnalysis())
            analysisPlugin->destroy();
    
        const auto shouldRemoveRawData = !mv::core()->isAboutToBeDestroyed() && dataset->isFull();
    
        _datasets.erase(it);
    
        if (shouldRemoveRawData)
            removeRawData(rawDataName);

	    if (!mv::core()->isAboutToBeDestroyed()) {
		    events().notifyDatasetRemoved(datasetId, datasetDataType);
    		emit datasetRemoved(datasetId);
	    }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove dataset from the data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to remove dataset from the data manager");
    }
}

void DataManager::removeDatasets(Datasets datasets)
{
    try {
#ifdef DATA_MANAGER_VERBOSE
        qDebug() << "Remove" << datasets.size() << "datasets from the data manager";
#endif

        mv::dataHierarchy().clearSelection();

        if (datasets.isEmpty())
            throw std::runtime_error("No datasets to remove");

        DataHierarchyItems selectedDataHierarchyItems;

        for (auto& dataset : datasets)
            selectedDataHierarchyItems << &dataset->getDataHierarchyItem();

        Datasets topLevelDatasets;

        for (const auto& dataset : datasets)
            if (!dataset->getDataHierarchyItem().isChildOf(selectedDataHierarchyItems))
                topLevelDatasets << dataset;

        DataHierarchyItems descendantDataHierarchyItems;

        for (auto topLevelDataset : topLevelDatasets)
            descendantDataHierarchyItems << topLevelDataset->getDataHierarchyItem().getChildren(true);

        Datasets datasetsToRemove;

        for (auto datasetToRemove : datasetsToRemove)
            datasetToRemove->lock();

        if (descendantDataHierarchyItems.isEmpty()) {
            datasetsToRemove = topLevelDatasets;
        }
        else {
            if (settings().getMiscellaneousSettings().getAskConfirmationBeforeRemovingDatasetsAction().isChecked()) {
                RemoveDatasetsDialog removeDatasetsDialog(datasets);

                if (removeDatasetsDialog.exec() == QDialog::Accepted)
                    datasetsToRemove = removeDatasetsDialog.getDatasetsToRemove();
            }
            else {
                DatasetsToRemoveModel datasetsToRemoveModel;
                DatasetsToRemoveFilterModel datasetsToRemoveFilterModel;
                
                datasetsToRemoveFilterModel.setSourceModel(&datasetsToRemoveModel);

                datasetsToRemoveModel.setDatasets(datasets);

                datasetsToRemove = datasetsToRemoveModel.getDatasetsToRemove();
            }
        }

        if (!datasetsToRemove.isEmpty()) {
            auto task = ModalTask(this, "Remove dataset(s)", Task::Status::Running);

            task.setSubtasks(datasetsToRemove.count());

            for (const auto& datasetToRemove : datasetsToRemove) {
                const auto datasetIndex = datasetsToRemove.indexOf(datasetToRemove);
                const auto datasetGuiName = datasetToRemove->getGuiName();

                task.setSubtaskStarted(datasetIndex, QString("Removing %1").arg(datasetGuiName));
                {
                    removeDataset(datasetToRemove);
                }
                task.setSubtaskFinished(datasetIndex, QString("Removed %1").arg(datasetGuiName));
            }

            task.setFinished();
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove dataset supervised", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to remove dataset supervised");
    }
}

void DataManager::removeDatasets(const QString& rawDataName)
{
    Datasets datasetsToRemove;

    for (const auto& dataset : _datasets)
        if (dataset->getRawDataName() == rawDataName)
            removeDataset(dataset.get());
}

Dataset<DatasetImpl> DataManager::createDerivedDataset(const QString& guiName, const Dataset<DatasetImpl>& sourceDataset, const Dataset<DatasetImpl>& parentDataset /*= Dataset<DatasetImpl>()*/, bool notify /*= true*/)
{
#ifdef DATA_MANAGER_VERBOSE
    qDebug() << "Create derived dataset" << guiName << sourceDataset->getGuiName();
#endif

    // Get the data type of the source dataset
    const auto dataType = sourceDataset->getDataType();

    // Create a new plugin of the given kind
    QString pluginName = mv::plugins().requestPlugin(dataType._type)->getName();

    auto rawData = getRawData(pluginName);

    // Create an initial full set, but no selection because it is shared with the source data
    auto derivedDataset = rawData->createDataSet();

    // Mark the full set as derived and set the GUI name
    derivedDataset->setSourceDataset(sourceDataset);
    derivedDataset->setText(guiName);

    // Set properties of the new set
    derivedDataset->setAll(true);

    addDataset(derivedDataset, !parentDataset.isValid() ? const_cast<Dataset<DatasetImpl>&>(sourceDataset) : const_cast<Dataset<DatasetImpl>&>(parentDataset), notify);

    derivedDataset->init();

    return Dataset<DatasetImpl>(*derivedDataset);
}

Dataset<DatasetImpl> DataManager::createSubsetFromSelection(const Dataset<DatasetImpl>& selection, const Dataset<DatasetImpl>& sourceDataset, const QString& guiName, const Dataset<DatasetImpl>& parentDataset, const bool& visible /*= true*/, bool notify /*= true*/)
{
    try
    {

#ifdef DATA_MANAGER_VERBOSE
        qDebug() << "Create subset from selection" << guiName << sourceDataset->getGuiName();
#endif

        // Create a subset with only the indices that were part of the selection set
        auto subset = selection->copy();

        // Assign source dataset to subset
        *subset = *const_cast<Dataset<DatasetImpl>&>(sourceDataset);

        subset->setText(guiName);

        // Set a pointer to the original full dataset, if the source is another subset, we take their pointer
        subset->makeSubsetOf(sourceDataset->isFull() ? sourceDataset : sourceDataset->_fullDataset);

        addDataset(subset, parentDataset, notify);

        subset->getDataHierarchyItem().setVisible(visible);
        subset->init();

        return subset;
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to create subset from selection", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to create subset from selection");
    }

    return {};
}

Dataset<> DataManager::getDataset(const QString& datasetId)
{
    try
    {
        if (datasetId.isEmpty())
            throw std::runtime_error("Dataset GUID is invalid");

        const auto it = std::find_if(_datasets.begin(), _datasets.end(), [datasetId](const auto& datasetPtr) -> bool {
            return datasetId == datasetPtr->getId();
        });

        if (it == _datasets.end())
            throw std::runtime_error(QString("Dataset with id %1 not found in database").arg(datasetId).toStdString());

        return (*it).get();
    }
    catch (std::exception& e)
    {
        qDebug() << QString("Unable to get set from data manager: %1").arg(e.what());
    }
    catch (...) {
        qDebug() << "Unable to get set from data manager due to unhandled exception";
    }

    return {};
}

Datasets DataManager::getAllDatasets(const std::vector<DataType>& dataTypes /*= std::vector<DataType>()*/) const
{
    QVector<Dataset<>> allDatasets;

    for (const auto& dataset : _datasets) {
        if (dataTypes.empty()) {
            allDatasets << dataset.get();
        }
        else {
            if (std::find(dataTypes.begin(), dataTypes.end(), dataset->getDataType()) != dataTypes.end())
                allDatasets << dataset.get();
        }
    }

    return allDatasets;
}

void DataManager::addSelection(const QString& rawDataName, Dataset<DatasetImpl> selection)
{
#ifdef DATA_MANAGER_VERBOSE
    qDebug() << "Add selection dataset for raw data" << rawDataName << "to the data manager";
#endif

    _selections.push_back(std::unique_ptr<DatasetImpl>(selection.get()));

    emit selectionAdded(selection);
}

Dataset<DatasetImpl> DataManager::getSelection(const QString& rawDataName)
{
    try
    {
        if (rawDataName.isEmpty())
            throw std::runtime_error("Raw data name is invalid");

        const auto it = std::find_if(_selections.begin(), _selections.end(), [rawDataName](const auto& selectionPtr) -> bool {
            return rawDataName == selectionPtr->getRawDataName();
        });

        if (it == _selections.end())
            throw std::runtime_error(QString("Selection set not found for raw data %1").arg(rawDataName).toStdString());

        return (*it).get();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get selection dataset from data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get selection dataset from data manager");
    }

    return {};
}

Datasets DataManager::getAllSelections()
{
    Datasets selections;

    for (const auto& dataset : _selections)
        selections << dataset.get();

    return selections;
}

void DataManager::removeSelection(const QString& rawDataName)
{
    try
    {

#ifdef DATA_MANAGER_VERBOSE
        qDebug() << "Remove selection dataset for raw data" << rawDataName << "from the data manager";
#endif

        auto it = std::find_if(_selections.begin(), _selections.end(), [rawDataName](const auto& selection) -> bool {
            return selection->getRawDataName() == rawDataName;
        });

        if (it != _selections.end()) {
            const auto selection    = (*it).get();
            const auto selectionId  = selection->getId();

#ifdef DATA_MANAGER_VERBOSE
            qDebug() << "Remove selection dataset" << selectionId;
#endif

            emit selectionAboutToBeRemoved(Dataset<DatasetImpl>(selection));
            {
                _selections.erase(it);
            }
            emit selectionRemoved(selectionId);
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove selection dataset from data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to remove selection dataset from data manager");
    }
}

mv::Dataset<mv::DatasetImpl> DataManager::groupDatasets(const Datasets& datasets, const QString& guiName /*= ""*/)
{
    try {
        const auto createGroupDataset = [this, &datasets](const QString& guiName) -> Dataset<DatasetImpl> {
            auto groupDataset = createDataset(datasets.first()->getRawDataKind(), guiName);

            groupDataset->setProxyMembers(datasets);

            return groupDataset;
        };

        if (guiName.isEmpty()) {
            if (Application::current()->getSetting("AskForGroupName", true).toBool()) {
                GroupDataDialog groupDataDialog(nullptr, datasets);

                groupDataDialog.open();

                QEventLoop eventLoop;
                QObject::connect(&groupDataDialog, &QDialog::finished, &eventLoop, &QEventLoop::quit);
                eventLoop.exec();

                if (groupDataDialog.result() == QDialog::Accepted)
                    return createGroupDataset(groupDataDialog.getGroupName());
                else
                    return Dataset<DatasetImpl>();
            }
            else {
                QStringList datasetNames;

                for (const auto& dataset : datasets)
                    datasetNames << dataset->text();

                return createGroupDataset(datasetNames.join("+"));
            }
        }
        else {
            return createGroupDataset(guiName);
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to group datasets", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to group datasets");
    }

    return Dataset<DatasetImpl>();
}

ToggleAction* DataManager::getSelectionGroupingAction()
{
    if (!projects().hasProject())
        return nullptr;
        
    return &projects().getCurrentProject()->getSelectionGroupingAction();
}

const DatasetsListModel& DataManager::getDatasetsListModel() const
{
    return *_datasetsListModel;
}

void DataManager::linkFilterModelToDatasetsListModel(QSortFilterProxyModel* filterModel)
{
    Q_ASSERT(filterModel);

    if (!filterModel)
        return;

    filterModel->setSourceModel(_datasetsListModel);
}

void DataManager::fromVariantMap(const QVariantMap& variantMap)
{
    AbstractDataManager::fromVariantMap(variantMap);
}

QVariantMap DataManager::toVariantMap() const
{
    QVariantMap variantMap = AbstractDataManager::toVariantMap();

    for (auto& dataset : _datasets)
        variantMap[dataset->getId()] = dataset->toVariantMap();

    return variantMap;
}

}
