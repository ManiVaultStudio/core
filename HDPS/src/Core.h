#ifndef HDPS_CORE_H
#define HDPS_CORE_H

#include "CoreInterface.h"
#include "PluginType.h"
#include "DataType.h"
#include "DataManager.h"

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
    void removeDataset(const QString datasetName);// override;

    const QString createDerivedData(const QString nameRequest, const QString sourceDatasetName) override;

    /**
     * Creates a copy of the given selection set and gives it a unique name based
     * on the name given to this function. Then adds the new set to the data manager
     * and notifies all data consumers of the new set.
     */
    QString createSubsetFromSelection(const DataSet& selection, const DataSet& parentSet, const QString newSetName) override;

    /**
     * Requests an instance of a data type plugin from the core which has the same
     * unique name as the given parameter. If no such instance can be found a fatal
     * error is thrown.
     */
    RawData& requestRawData(const QString name) override;

    /**
    * Request a dataset from the data manager by its name.
    */
    DataSet& requestData(const QString name) override;

    /**
     * Request a list of all datasets in the core
     */
    const std::vector<const DataSet*> requestDatasets() override;

    void registerDatasetChanged(QString datasetName, DataChangedFunction func) override;
    void registerSelectionChanged(QString datasetName, SelectionChangedFunction func) override;

    void registerDataTypeAdded(DataType dataType, DataAddedFunction func) override;
    void registerDataTypeChanged(DataType dataType, DataChangedFunction func) override;
    void registerDataTypeRemoved(DataType dataType, DataRemovedFunction func) override;

    /** Notify all data consumers that a new dataset has been added to the core. */
    void notifyDataAdded(const QString name) override;
    /** Notify all data consumers that a dataset has been changed. */
    void notifyDataChanged(const QString name) override;

    /** Notify all data consumers that a selection has changed. */
    void notifySelectionChanged(const QString dataName) override;

    /**
    * Returns a reference to the main window for adding widgets to it.
    */
    gui::MainWindow& gui() const;
    
protected:
    /**
    * Requests the selection set belonging to the raw dataset with the given name.
    */
    DataSet& requestSelection(const QString rawdataName) override;

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

    std::unordered_map<QString, std::vector<DataChangedFunction>> datasetChangedListeners;
    std::unordered_map<QString, std::vector<SelectionChangedFunction>> selectionChangedListeners;
    std::unordered_map<DataType, std::vector<DataAddedFunction>> dataTypeAddedListeners;
    std::unordered_map<DataType, std::vector<DataChangedFunction>> dataTypeChangedListeners;
    std::unordered_map<DataType, std::vector<DataRemovedFunction>> dataTypeRemovedListeners;
};

} // namespace hdps

#endif // HDPS_CORE_H
