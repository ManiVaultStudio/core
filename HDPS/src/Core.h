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

namespace util
{
    template<typename DatasetType>
    class DatasetRef;
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
     * unique name of the data set linked with the raw data
     * @param kind Kind of plugin
     * @param datasetGuiName Name of the added dataset in the GUI
     * @param parentDataSet Pointer to parent dataset in the data hierarchy (root if nullptr)
     * @return Reference to the added dataset
     */
    DataSet& addData(const QString& kind, const QString& dataSetGuiName, const DataSet* parentDataSet = nullptr) override;

    /**
     * Removes one or more datasets
     * Other datasets derived from this dataset are  converted to non-derived data.
     * Notifies all plug-ins of the removed dataset automatically
     * @param datasets Datasets to remove
     * @param recursively Remove datasets recursively
     */
    void removeDatasets(const QVector<DataSet*> datasets, const bool& recursively = false) override;

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
     * @param sourceDataset Source dataset
     * @param guiName GUI name of the subset
     * @param parentDataset Pointer to the parent dataset in the data hierarchy (sourceSetName if is used if empty)
     * @param visible Whether the new dataset is visible in the user interface
     * @return Subset
     */
    DataSet& createSubsetFromSelection(const DataSet& selection, const DataSet& sourceDataset, const QString& guiName, const DataSet* parentDataset = nullptr, const bool& visible = true) override;

public: // Data requests

    /**
     * Requests a dataset from the core by dataset globally unique identifier (if no such instance can be found a fatal error is thrown)
     * @param dataSetId Globally unique identifier of the dataset
     * @return Reference to data set
     */
    DataSet& requestData(const QString& dataSetId) override;

    /**
     * Returns all data sets that are present in the core, filtered by data type(s)
     * Returns all data sets in case of an empty filter
     * @param dataTypes Data types to filter
     * @return Vector of pointers to datasets
     */
    QVector<DataSet*> requestAllDataSets(const QVector<DataType>& dataTypes = QVector<DataType>()) override;

public: // Analysis

    /**
     * Request an analysis by its kind
     * @param kind Type of analysis
     * @return Reference to created plugin
     */
    plugin::Plugin& requestAnalysis(const QString& kind) override;

    /**
     * Analyze a dataset
     * @param kind Type of analysis
     * @param dataSet Reference  to dataset to analyze
     */
    const void analyzeDataset(const QString& kind, DataSet& dataSet) override;

public: // Import/export

    /**
     * Imports a dataset
     * @param kind Type of import plugin
     */
    const void importDataset(const QString& kind) override;

    /**
     * Exports a dataset
     * @param kind Type of export plugin
     * @param dataSet Reference to dataset to export
     */
    const void exportDataset(const QString& kind, DataSet& dataSet) override;

public: // Data hierarchy

    /**
     * Get data hierarchy item by dataset globally unique identifier
     * @param dataSetId Globally unique identifier of the dataset
     * @return Pointer to data hierarchy item
     */
    DataHierarchyItem* getDataHierarchyItem(const QString& dataSetId) override;

public: // Plugin queries

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

    /**
     * Get plugin icon from plugin kind
     * @param pluginKind Kind of plugin
     * @return Plugin icon name of the plugin, null icon the plugin kind was not found
     */
    QIcon getPluginIcon(const QString& pluginKind) const override;

public: // Events & notifications

    /**
     * Notify listeners that a new dataset has been added to the core
     * @param dataset Reference to the dataset that was added
     */
    void notifyDataAdded(DataSet& dataset) override;

    /**
     * Notify listeners that a dataset is about to be removed
     * @param dataset Reference to the dataset which is about to be removed
     */
    void notifyDataAboutToBeRemoved(DataSet& dataset) override;

    /**
     * Notify listeners that a dataset is removed
     * @param datasetId Globally unique identifier of the dataset that was removed
     * @param dataType Type of the data
     */
    void notifyDataRemoved(const QString& datasetId, const DataType& dataType) override;

    /**
     * Notify listeners that a dataset has changed
     * @param dataset Reference to the dataset of which the data changed
     */
    void notifyDataChanged(DataSet& dataset) override;

    /**
     * Notify listeners that a selection has changed
     * @param dataset Reference to the dataset of which the selection changed
     */
    void notifySelectionChanged(DataSet& dataset) override;

    /**
     * Notify listeners that a dataset GUI name has changed
     * @param dataset Reference to dataset of which the GUI name changed
     * @param previousGuiName Previous dataset name
     */
    void notifyGuiNameChanged(DataSet& dataset, const QString& previousGuiName) override;

    /**
     * Register an event listener
     * @param eventListener Pointer to event listener to register
     */
    void registerEventListener(EventListener* eventListener) override;

    /**
     * Unregister an event listener
     * @param eventListener Pointer to event listener to unregister
     */
    void unregisterEventListener(EventListener* eventListener) override;

public:

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
    plugin::RawData& requestRawData(const QString name) override;

    /**
    * Requests the selection set belonging to the raw dataset with the given name.
    */
    DataSet& requestSelection(const QString rawdataName) override;

private:

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
