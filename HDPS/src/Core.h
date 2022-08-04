#ifndef HDPS_CORE_H
#define HDPS_CORE_H

#include "CoreInterface.h"
#include "PluginType.h"
#include "DataType.h"
#include "DataManager.h"
#include "DataHierarchyManager.h"

#include <event/EventListener.h>

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
    class Dataset;
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

    /** Aliases */
    using UniquePtrPlugin   = std::unique_ptr<plugin::Plugin>;
    using UniquePtrsPlugin  = std::vector<UniquePtrPlugin>;

public:

    /**
     * Constructor
     * @param mainWindow Reference to the main window
     */
    Core(gui::MainWindow& mainWindow);

    /** Destructor */
    ~Core();

public: // Data access

    /**
     * Requests the plugin manager to create new RawData of the given kind
     * The manager will add the raw data to the core and return the unique name of the data set linked with the raw data
     * @param kind Kind of plugin
     * @param datasetGuiName Name of the added dataset in the GUI
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (root if not valid)
     * @return Smart pointer to the added dataset
     */
    Dataset<DatasetImpl> addDataset(const QString& kind, const QString& dataSetGuiName, const Dataset<DatasetImpl>& parentDataset = Dataset<DatasetImpl>()) override;

    /**
     * Removes a single dataset
     * @param dataset Smart pointer to the dataset to remove
     */
    void removeDataset(Dataset<DatasetImpl> dataset) override final;

    /** Removes all currently loaded datasets */
    void removeAllDatasets() override final;

    /**
     * Copies a dataset and adds it to the data hierarchy
     * @param dataset Smart pointer to dataset to copy
     * @param datasetGuiName Name of the added dataset in the GUI
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (root if not valid)
     * @return Smart pointer to the copied dataset
     */
    Dataset<DatasetImpl> copyDataset(const Dataset<DatasetImpl>& dataset, const QString& dataSetGuiName, const Dataset<DatasetImpl>& parentDataset = Dataset<DatasetImpl>()) override;

    /**
     * Creates a dataset derived from a source dataset.
     * @param guiName GUI name for the new dataset from the core
     * @param sourceDataset Smart pointer to the source dataset from which this dataset will be derived
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (will attach to root in hierarchy if not valid)
     * @return Smart pointer to the created derived dataset
     */
    Dataset<DatasetImpl> createDerivedDataset(const QString& guiName, const Dataset<DatasetImpl>& sourceDataset, const Dataset<DatasetImpl>& parentDataset = Dataset<DatasetImpl>()) override;

    /**
     * Creates a copy of the given selection set, adds the new set to the data manager and notifies all data consumers of the new set
     * @param selection Smart pointer to the selection set
     * @param sourceDataset Smart pointer to the source dataset
     * @param guiName GUI name of the subset
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (sourceSetName if not valid)
     * @param visible Whether the new dataset is visible in the user interface
     * @return Smart pointer to the created subset
     */
    Dataset<DatasetImpl> createSubsetFromSelection(const Dataset<DatasetImpl>& selection, const Dataset<DatasetImpl>& sourceDataset, const QString& guiName, const Dataset<DatasetImpl>& parentDataset, const bool& visible = true) override;

    /**
     * Requests a dataset from the core by dataset GUID (if no such instance can be found a fatal error is thrown)
     * @param datasetGuid GUID of the dataset
     * @return Smart pointer to the dataset
     */
    Dataset<DatasetImpl> requestDataset(const QString& datasetGuid) override;

    /**
     * Returns all data sets that are present in the core, filtered by data type(s)
     * Returns all data sets in case of an empty filter
     * @param dataTypes Data types to filter
     * @return Vector of references to datasets
     */
    QVector<Dataset<DatasetImpl>> requestAllDataSets(const QVector<DataType>& dataTypes = QVector<DataType>()) override;

    /**
     * Get data manager
     * @return Reference to the data manager
     */
    const DataManager& getDataManager() const;

protected: // Data access

    /**
     * Requests an instance of a data type plugin from the core which has the same unique name as the given parameter, if no such instance can be found a fatal error is thrown
     * @param datasetName Name of the dataset
     */
    plugin::RawData& requestRawData(const QString& name) override;

    /**
     * Request a selection from the data manager by its corresponding raw data name.
     * @param rawdataName Name of the raw data
     */
    Dataset<DatasetImpl> requestSelection(const QString& rawDataName) override;

public: // Data grouping

    /**
     * Groups \p datasets into one dataset
     * @param datasets Two or more datasets to group  (if empty, the user will be prompted for a name)
     * @return Smart pointer to created group dataset
     */
    Dataset<DatasetImpl> groupDatasets(const Datasets& datasets, const QString& guiName = "");

public: // Plugin creation

    /**
     * Create a plugin of \p kind
     * @param kind Kind of plugin (name of the plugin)
     * @param datasets Zero or more datasets upon which the plugin is based (e.g. analysis plugin)
     * @return Pointer to created plugin
     */
    plugin::Plugin* requestPlugin(const QString& kind, const Datasets& datasets = Datasets()) override;

public: // Plugin queries

    /**
     * Get plugin trigger actions by plugin type and datasets
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @param datasets Input datasets
     * @return List of plugin trigger actions
     */
    QList<gui::PluginTriggerAction*> getPluginTriggerActionsByPluginTypeAndDatasets(const plugin::Type& pluginType, const Datasets& datasets) const;

    /**
     * Get plugin trigger actions by plugin kind and datasets
     * @param pluginKind Kind of plugin
     * @param datasets Input datasets
     * @return List of plugin trigger actions
     */
    QList<gui::PluginTriggerAction*> getPluginTriggerActionsByPluginKindAndDatasets(const QString& pluginKind, const Datasets& datasets) const;

    /**
     * Get plugin GUI name from plugin kind
     * @param pluginKind Kind of plugin
     * @return GUI name of the plugin, empty if the plugin kind was not found
     */
    QString getPluginGuiName(const QString& pluginKind) const override;

    /**
     * Get plugin icon from plugin kind
     * @param pluginKind Kind of plugin
     * @return Plugin icon name of the plugin, null icon the plugin kind was not found
     */
    QIcon getPluginIcon(const QString& pluginKind) const override;

public: // Data hierarchy

    /** Get a reference to the data hierarchy manager */
    DataHierarchyManager& getDataHierarchyManager() override;

    /**
     * Get data hierarchy item by dataset GUI
     * @param datasetGuid Globally unique identifier of the dataset
     * @return Reference to data hierarchy item
     */
    DataHierarchyItem& getDataHierarchyItem(const QString& datasetGuid) override;

public: // Dataset grouping

    /** Get whether dataset grouping is enabled or not */
    bool isDatasetGroupingEnabled() const override;

    /** Get whether dataset grouping is enabled or not */
    void setDatasetGroupingEnabled(const bool& datasetGroupingEnabled) override;

public: // Events & notifications

    /**
     * Notify listeners that a new dataset has been added to the core
     * @param dataset Smart pointer to the dataset that was added
     */
    void notifyDatasetAdded(const Dataset<DatasetImpl>& dataset) override;

    /**
     * Notify listeners that a dataset is about to be removed
     * @param dataset Smart pointer to the dataset which is about to be removed
     */
    void notifyDatasetAboutToBeRemoved(const Dataset<DatasetImpl>& dataset) override;

    /**
     * Notify listeners that a dataset is removed
     * @param datasetGuid GUID of the dataset that was removed
     * @param dataType Type of the data
     */
    void notifyDatasetRemoved(const QString& datasetGuid, const DataType& dataType) override;

    /**
     * Notify listeners that a dataset has changed
     * @param dataset Smart pointer to the dataset of which the data changed
     */
    void notifyDatasetChanged(const Dataset<DatasetImpl>& dataset) override;

    /**
     * Notify listeners that data selection has changed
     * @param dataset Smart pointer to the dataset of which the selection changed
     */
    void notifyDatasetSelectionChanged(const Dataset<DatasetImpl>& dataset) override;

    /**
     * Notify all listeners that a dataset GUI name has changed
     * @param dataset Smart pointer to the dataset of which the GUI name changed
     * @param previousGuiName Previous dataset name
     */
    void notifyDatasetGuiNameChanged(const Dataset<DatasetImpl>& dataset, const QString& previousGuiName) override;

    /**
     * Notify all listeners that a dataset is locked
     * @param dataset Smart pointer to the dataset
     */
    void notifyDatasetLocked(const Dataset<DatasetImpl>& dataset) override;

    /**
     * Notify all listeners that a dataset is unlocked
     * @param dataset Smart pointer to the dataset
     */
    void notifyDatasetUnlocked(const Dataset<DatasetImpl>& dataset) override;

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

    /** Returns a reference to the main window for adding widgets to it */
    gui::MainWindow& gui() const;

    /** Initializes the core; loads all plugins from the designated plugin directory */
    void init();

    /** Returns a reference to the internal data manager */
    DataManager& getDataManager() {
        return *_dataManager;
    }

    /** Adds the given plugin to the list of plugins kept by the core */
    void addPlugin(plugin::Plugin* plugin);

private:

    /** Destroys all plug-ins kept by the core */
    void destroyPlugins();

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

private:
    gui::MainWindow&                                                        _mainWindow;                /** Reference to the main window */
    std::unique_ptr<plugin::PluginManager>                                  _pluginManager;             /** Plugin manager responsible for loading plug-ins and adding them to the core. */
    std::unique_ptr<DataManager>                                            _dataManager;               /** Data manager responsible for storing data sets and data selections */
    std::unique_ptr<DataHierarchyManager>                                   _dataHierarchyManager;      /** Internal hierarchical data tree */
    std::unordered_map<plugin::Type, UniquePtrsPlugin, plugin::TypeHash>    _plugins;                   /** List of plugin instances currently present in the application. Instances are stored by type. */
    std::vector<EventListener*>                                             _eventListeners;            /** List of classes listening for core events */

    friend class DataHierarchyManager;
};

} // namespace hdps

#endif // HDPS_CORE_H
