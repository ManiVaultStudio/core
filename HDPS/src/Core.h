#ifndef HDPS_CORE_H
#define HDPS_CORE_H

#include "CoreInterface.h"
#include "PluginType.h"
#include "DataType.h"
#include "DataManager.h"
#include "event/EventListener.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace hdps
{

namespace plugin
{
    class PluginManager;
    class Plugin;
    class DataConsumer;
}

namespace gui
{
    class MainWindow;
}

struct AnalysisNotFoundException : public std::exception
{
public:
    AnalysisNotFoundException(QString analysisName) :
        message((QString("Failed to find an analysis with name: ") + analysisName).toStdString())
    { }

    const char* what() const throw () override
    {
        return message.c_str();
    }

private:
    std::string message;
};

class Core : public CoreInterface
{
public:
    Core(gui::MainWindow& mainWindow);
    ~Core();

    /**
    * Initializes the core. Loads all plugins from the designated plugin directory.
    */
    void init();

    /**
     * Returns a reference to the internal data manager
     */
    DataManager& getDataManager()
    {
        return *_dataManager;
    }

    /**
    * Adds the given plugin to the list of plugins kept by the core.
    */
    void addPlugin(plugin::Plugin* plugin);

    /**
     * Requests the plugin manager to create new RawData of the given kind.
     * The manager will add the raw data to the core and return the
     * unique name of the data set linked with the raw data.
     */
    const QString addData(const QString kind, const QString nameRequest) override;

    /**
     * Removes a Dataset object. Other datasets derived from this datasets are
     * converted to non-derived data.
     * Notifies all plug-ins of the removed data set automatically.
     */
    void removeDataset(const QString datasetName) override;

    /**
     * Creates a dataset derived from a source dataset.
     * @param nameRequest Preferred name for the new dataset from the core (May be changed if not unique)
     * @param sourceDatasetName Name of the source dataset from which this dataset will be derived
     */
    const QString createDerivedData(const QString nameRequest, const QString sourceDatasetName) override;

    /**
     * Creates a copy of the given selection set and gives it a unique name based
     * on the name given to this function. Then adds the new set to the data manager
     * and notifies all data consumers of the new set.
     */
    QString createSubsetFromSelection(const DataSet& selection, const DataSet& parentSet, const QString newSetName) override;

    /**
     * Requests a dataset from the core which has the same unique name
     * as the given parameter. If no such instance can be found a fatal
     * error is thrown.
     */
    DataSet& requestData(const QString datasetName) override;

    /**
    * Request an analysis by its name.
    */
    plugin::Plugin& requestAnalysis(const QString name) override;

    /**
    * Request all data set names.
    */
    virtual std::vector<QString> requestAllDataNames() override;

    /**
    * Request names for data sets of a specific type.
    */
    virtual std::vector<QString> requestAllDataNames(const std::vector<DataType> dataTypes) override;

    /** Notify all data consumers that a new dataset has been added to the core. */
    void notifyDataAdded(const QString datasetName) override;
    /** Notify all data consumers that a dataset has been changed. */
    void notifyDataChanged(const QString datasetName) override;

    /** Notify all data consumers that a selection has changed. */
    void notifySelectionChanged(const QString datasetName) override;

    /** Notify all event listeners that a dataset has been renamed. */
    void notifyDataRenamed(const QString oldName, const QString newName) override;

    /**
    * Returns a reference to the main window for adding widgets to it.
    */
    gui::MainWindow& gui() const;
    
protected:
    /**
     * Requests an instance of a data type plugin from the core which has the same
     * unique name as the given parameter. If no such instance can be found a fatal
     * error is thrown.
     */
    RawData& requestRawData(const QString name) override;

    /**
    * Requests the selection set belonging to the raw dataset with the given name.
    */
    DataSet& requestSelection(const QString rawdataName) override;

    void registerEventListener(EventListener* eventListener) override;

    void unregisterEventListener(EventListener* eventListener) override;

private:
    /** Notify all data consumers that a dataset has been removed. */
    void notifyDataRemoved(const QString name);

    /** Destroys all plug-ins kept by the core */
    void destroyPlugins();

private:
    gui::MainWindow& _mainWindow;

    /** Plugin manager responsible for loading plug-ins and adding them to the core. */
    std::unique_ptr<plugin::PluginManager> _pluginManager;

    /** Data manager responsible for storing data sets and data selections */
    std::unique_ptr<DataManager> _dataManager;

    /** List of plugin instances currently present in the application. Instances are stored by type. */
    std::unordered_map<plugin::Type, std::vector<std::unique_ptr<plugin::Plugin>>, plugin::TypeHash> _plugins;

    /** List of classes listening for core events */
    std::vector<EventListener*> _eventListeners;
};

} // namespace hdps

#endif // HDPS_CORE_H
