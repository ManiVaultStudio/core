// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Core.h"

#include "MainWindow.h"
#include "PluginManager.h"
#include "GroupDataDialog.h"
#include "ModalTask.h"
#include "ForegroundTask.h"

#include <LoaderPlugin.h>
#include <WriterPlugin.h>
#include <ViewPlugin.h>
#include <AnalysisPlugin.h>
#include <RawData.h>
#include <Set.h>
#include <PluginFactory.h>
#include <ViewPlugin.h>
#include <event/Event.h>
#include <util/Exception.h>

#include <algorithm>

#include <QEventLoop>

//#define CORE_VERBOSE

using namespace mv;
using namespace mv::util;
using namespace mv::plugin;
using namespace mv::gui;

namespace mv {

Core::Core() :
    CoreInterface()
{
    _datasetGroupingEnabled = Application::current()->getSetting("Core/DatasetGroupingEnabled", false).toBool();
}

Core::~Core()
{
    getPluginManager().reset();
}

void Core::createManagers()
{
    _managers.resize(static_cast<int>(ManagerType::Count));

    _managers[static_cast<int>(ManagerType::Actions)]       = new ActionsManager();
    _managers[static_cast<int>(ManagerType::Plugins)]       = new PluginManager();
    _managers[static_cast<int>(ManagerType::Events)]        = new EventManager();
    _managers[static_cast<int>(ManagerType::Data)]          = new DataManager();
    _managers[static_cast<int>(ManagerType::DataHierarchy)] = new DataHierarchyManager();
    _managers[static_cast<int>(ManagerType::Tasks)]         = new TaskManager();
    _managers[static_cast<int>(ManagerType::Workspaces)]    = new WorkspaceManager();
    _managers[static_cast<int>(ManagerType::Projects)]      = new ProjectManager();
    _managers[static_cast<int>(ManagerType::Settings)]      = new SettingsManager();

    CoreInterface::setManagersCreated();
}

void Core::initialize()
{
    if (isInitialized())
        return;

    CoreInterface::setAboutToBeInitialized();
    {
        auto& loadCoreManagersTask = Application::current()->getStartupTask().getLoadCoreManagersTask();

        QStringList subtasks;

        for (auto& manager : _managers)
            subtasks << manager->getSerializationName();

        loadCoreManagersTask.setSubtasks(subtasks);
        loadCoreManagersTask.setRunning();

        for (auto& manager : _managers) {
            loadCoreManagersTask.setSubtaskStarted(manager->getSerializationName(), "Initializing " + manager->getSerializationName().toLower() + " manager");
            {
                manager->initialize();
            }
            loadCoreManagersTask.setSubtaskFinished(manager->getSerializationName(), "Initializing " + manager->getSerializationName().toLower() + " manager");

            QCoreApplication::processEvents();
        }
        
        loadCoreManagersTask.setFinished();
    }
    CoreInterface::setInitialized();
}

void Core::reset()
{
    for (auto& manager : _managers)
        manager->reset();
}

void Core::addPlugin(plugin::Plugin* plugin)
{
    switch (plugin->getType())
    {
        // If the plugin is RawData, then add it to the data manager
        case plugin::Type::DATA:
        {
            getDataManager().addRawData(dynamic_cast<plugin::RawData*>(plugin));
            break;
        }

        case plugin::Type::VIEW:
            break;
    }

    // Initialize the plugin after it has been added to the core
    plugin->init();

    switch (plugin->getType())
    {
        case plugin::Type::ANALYSIS:
        {
            auto analysisPlugin = dynamic_cast<plugin::AnalysisPlugin*>(plugin);

            if (analysisPlugin)
                events().notifyDatasetAdded(analysisPlugin->getOutputDataset());

            break;
        }

        case plugin::Type::LOADER:
        {
            try
            {
                dynamic_cast<plugin::LoaderPlugin*>(plugin)->loadData();
            }
            catch (plugin::DataLoadException e)
            {
                QMessageBox messageBox;
                messageBox.critical(0, "Error", e.what());
                messageBox.setFixedSize(500, 200);
            }

            break;
        }

        case plugin::Type::WRITER:
        {
            dynamic_cast<plugin::WriterPlugin*>(plugin)->writeData();

            break;
        }

    }
}

Dataset<DatasetImpl> Core::addDataset(const QString& kind, const QString& dataSetGuiName, const Dataset<DatasetImpl>& parentDataset /*= Dataset<DatasetImpl>()*/, const QString& id /*= ""*/)
{
    // Create a new plugin of the given kind
    QString rawDataName = plugins().requestPlugin(kind)->getName();

    const auto rawData = mv::data().getRawData(rawDataName);

    // Create an initial full set and an empty selection belonging to the raw data
    auto fullSet    = rawData->createDataSet(id);
    auto selection  = rawData->createDataSet();

    // Set the properties of the new sets
    fullSet->setText(dataSetGuiName);
    fullSet->setAll(true);

    fullSet->getTask().setName(dataSetGuiName);

    selection->getTask().setName(QString("%1_selection").arg(dataSetGuiName));

    // Set pointer of full dataset to itself just to avoid having to be wary of this not being set
    fullSet->_fullDataset = fullSet;

    mv::data().addDataset(fullSet, parentDataset);
    mv::data().addSelection(rawDataName, selection);

    fullSet->init();

    return fullSet;
}

Dataset<DatasetImpl> Core::createDerivedDataset(const QString& guiName, const Dataset<DatasetImpl>& sourceDataset, const Dataset<DatasetImpl>& parentDataset /*= Dataset<DatasetImpl>()*/)
{
    // Get the data type of the source dataset
    const auto dataType = sourceDataset->getDataType();

    // Create a new plugin of the given kind
    QString pluginName = getPluginManager().requestPlugin(dataType._type)->getName();

    auto rawData = mv::data().getRawData(pluginName);

    // Create an initial full set, but no selection because it is shared with the source data
    auto derivedDataset = rawData->createDataSet();

    // Mark the full set as derived and set the GUI name
    derivedDataset->setSourceDataSet(sourceDataset);
    derivedDataset->setText(guiName);

    // Set properties of the new set
    derivedDataset->setAll(true);

    mv::data().addDataset(derivedDataset, !parentDataset.isValid() ? const_cast<Dataset<DatasetImpl>&>(sourceDataset) : const_cast<Dataset<DatasetImpl>&>(parentDataset));

    derivedDataset->init();

    return Dataset<DatasetImpl>(*derivedDataset);
}

Dataset<DatasetImpl> Core::createSubsetFromSelection(const Dataset<DatasetImpl>& selection, const Dataset<DatasetImpl>& sourceDataset, const QString& guiName, const Dataset<DatasetImpl>& parentDataset, const bool& visible /*= true*/)
{
    try
    {
        // Create a subset with only the indices that were part of the selection set
        auto subset = selection->copy();

        // Assign source dataset to subset
        *subset = *const_cast<Dataset<DatasetImpl>&>(sourceDataset);

        subset->setAll(false);
        subset->setText(guiName);

        // Set a pointer to the original full dataset, if the source is another subset, we take their pointer
        subset->_fullDataset = sourceDataset->isFull() ? sourceDataset : sourceDataset->_fullDataset;

        mv::data().addDataset(subset, parentDataset, visible);

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

AbstractManager* Core::getManager(const ManagerType& managerType)
{
    return _managers[static_cast<int>(managerType)];
}

AbstractActionsManager& Core::getActionsManager()
{
    return *static_cast<AbstractActionsManager*>(getManager(ManagerType::Actions));
}

AbstractPluginManager& Core::getPluginManager()
{
    return *static_cast<AbstractPluginManager*>(getManager(ManagerType::Plugins));
}

AbstractEventManager& Core::getEventManager()
{
    return *static_cast<AbstractEventManager*>(getManager(ManagerType::Events));
}

AbstractDataManager& Core::getDataManager()
{
    return *static_cast<AbstractDataManager*>(getManager(ManagerType::Data));
}

AbstractDataHierarchyManager& Core::getDataHierarchyManager()
{
    return *static_cast<AbstractDataHierarchyManager*>(getManager(ManagerType::DataHierarchy));
}

AbstractWorkspaceManager& Core::getWorkspaceManager()
{
    return *static_cast<AbstractWorkspaceManager*>(getManager(ManagerType::Workspaces));
}

AbstractTaskManager& Core::getTaskManager()
{
    return *static_cast<AbstractTaskManager*>(getManager(ManagerType::Tasks));
}

AbstractProjectManager& Core::getProjectManager()
{
    return *static_cast<AbstractProjectManager*>(getManager(ManagerType::Projects));
}

AbstractSettingsManager& Core::getSettingsManager()
{
    return *static_cast<AbstractSettingsManager*>(getManager(ManagerType::Settings));
}

bool Core::isDatasetGroupingEnabled() const
{
    return _datasetGroupingEnabled;
}

Dataset<DatasetImpl> Core::groupDatasets(const Datasets& datasets, const QString& guiName /*= ""*/)
{
    try {
        const auto createGroupDataset = [this, &datasets](const QString& guiName) -> Dataset<DatasetImpl> {
            auto groupDataset = addDataset(datasets.first()->getRawDataKind(), guiName);

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
        exceptionMessageBox("Unable to group data", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to group data");
    }
        
    return Dataset<DatasetImpl>();
}

void Core::setDatasetGroupingEnabled(const bool& datasetGroupingEnabled)
{
    if (datasetGroupingEnabled == _datasetGroupingEnabled)
        return;

    _datasetGroupingEnabled = datasetGroupingEnabled;

    // Save the setting
    Application::current()->setSetting("Core/DatasetGroupingEnabled", _datasetGroupingEnabled);
}

}
