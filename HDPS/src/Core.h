#ifndef HDPS_CORE_H
#define HDPS_CORE_H

#include "CoreInterface.h"
#include "PluginType.h"
#include "DataType.h"
#include "DataManager.h"
#include "DataHierarchyManager.h"

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
    using UniquePtrPlugin   = std::unique_ptr<plugin::Plugin>;
    using UniquePtrsPlugin  = std::vector<UniquePtrPlugin>;

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

    /** Get the data hierarchy manager */
    DataHierarchyManager& getDataHierarchyManager() override;

    /**
    * Adds the given plugin to the list of plugins kept by the core.
    */
    void addPlugin(plugin::Plugin* plugin);

    /**
     * Requests the plugin manager to create new RawData of the given kind.
     * The manager will add the raw data to the core and return the
     * unique name of the data set linked with the raw data.
     * @param parentDatasetName Name of the parent dataset in the data hierarchy
     */
    const QString addData(const QString kind, const QString nameRequest, const QString& parentDatasetName = "") override;

    /**
     * Removes one or more datasets. Other datasets derived from this dataset are
     * converted to non-derived data.
     * Notifies all plug-ins of the removed dataset automatically.
     * @param datasetNames Names of the datasets to remove
     * @param recursively Remove datasets recursively
     */
    void removeDatasets(const QStringList& datasetNames, const bool& recursively = false) override;

    /**
     * Renames a dataset
     * @param currentDatasetName Current name of the dataset
     * @param intendedDatasetName Intended name of the dataset
     * @return New name of the dataset
     */
    QString renameDataset(const QString& currentDatasetName, const QString& intendedDatasetName) override;

    /**
     * Creates a dataset derived from a source dataset.
     * @param nameRequest Preferred name for the new dataset from the core (May be changed if not unique)
     * @param sourceDatasetName Name of the source dataset from which this dataset will be derived
     * @param dataHierarchyParent Name of the parent in the data hierarchy (sourceDatasetName if is used if empty)
     * @param visible Whether the new dataset is visible in the user interface
     */
    const QString createDerivedData(const QString& nameRequest, const QString& sourceDatasetName, const QString& dataHierarchyParent = "") override;

    /**
     * Creates a copy of the given selection set and gives it a unique name based
     * on the name given to this function. Then adds the new set to the data manager
     * and notifies all data consumers of the new set.
     * @param selection Selection set
     * @param sourceSet Source dataset
     * @param newSetName Intended name of the subset
     * @param dataHierarchyParent Name of the parent in the data hierarchy (sourceDatasetName if is used if empty)
     * @param visible Whether the new dataset is visible in the user interface
     */
    QString createSubsetFromSelection(const DataSet& selection, const DataSet& sourceSet, const QString newSetName, const QString dataHierarchyParent = "", const bool& visible = true) override;

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

    /**
     * Analyzes a dataset
     * @param kind Name of the analysis plugin
     * @param datasetName Name of the dataset to analyze
     */
    const void analyzeDataset(const QString kind, const QString& datasetName) override;

    /**
     * Imports a dataset
     * @param importKind Type of import plugin
     */
    const void importDataset(const QString importKind) override;

    /**
     * Exports a dataset
     * @param kind Name of exporter plugin
     * @param datasetName Name of the dataset to export
     */
    const void exportDataset(const QString kind, const QString& datasetName) override;

    /**
     * Get a list of plugin kinds (names) given a plugin type and data type(s)
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @param dataTypes Types of data that the plugin should be compatible with (data type ignored when empty)
     * @return List of compatible plugin kinds that can handle the data type
     */
    QStringList getPluginKindsByPluginTypeAndDataTypes(const plugin::Type& pluginType, const QVector<DataType>& dataTypes = QVector<DataType>()) const override;

    /**
     * Get plugin GUI name from plugin kind
     * @param pluginKind Kind of plugin
     * @param GUI name of the plugin, empty if the plugin kind was not found
     */
    QString getPluginGuiName(const QString& pluginKind) const override;

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
    
    /**
     * Get hierarchy item by dataset name
     * @param datasetName Name of the dataset
     * @return Pointer to data hierarchy item
     */
    DataHierarchyItem* getDataHierarchyItem(const QString& datasetName) override;

protected:
    /**
     * Requests an instance of a data type plugin from the core which has the same
     * unique name as the given parameter. If no such instance can be found a fatal
     * error is thrown.
     */
    plugin::RawData& requestRawData(const QString name) override;

    /**
    * Requests the selection set belonging to the raw dataset with the given name.
    */
    DataSet& requestSelection(const QString rawdataName) override;

    void registerEventListener(EventListener* eventListener) override;

    void unregisterEventListener(EventListener* eventListener) override;

private:

    /**
     * Notify all event listeners that a dataset has been removed
     * @param dataType Type of the data
     * @param name Name of the dataset
     */
    void notifyDataRemoved(const DataType& dataType, const QString name);

    /** Destroys all plug-ins kept by the core */
    void destroyPlugins();

private:
    gui::MainWindow&                                                        _mainWindow;                /** Reference to the main window */
    std::unique_ptr<plugin::PluginManager>                                  _pluginManager;             /** Plugin manager responsible for loading plug-ins and adding them to the core. */
    std::unique_ptr<DataManager>                                            _dataManager;               /** Data manager responsible for storing data sets and data selections */
    std::unique_ptr<DataHierarchyManager>                                   _dataHierarchyManager;      /** Internal hierarchical data tree */
    std::unordered_map<plugin::Type, UniquePtrsPlugin, plugin::TypeHash>    _plugins;                   /** List of plugin instances currently present in the application. Instances are stored by type. */
    std::vector<EventListener*>                                             _eventListeners;            /** List of classes listening for core events */
};

} // namespace hdps

#endif // HDPS_CORE_H
