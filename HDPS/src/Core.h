#pragma once

#include "CoreInterface.h"

#include <exception>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace hdps
{

struct PluginNotFoundException : public std::exception
{
public:
  explicit PluginNotFoundException(const QString& pluginName) :
        message((QString::fromLatin1("Failed to find a plugin with name: ") + pluginName).toStdString())
    { }

    const char* what() const noexcept override
    {
        return message.c_str();
    }

private:
    std::string message;
};


namespace plugin
{
    class PluginManager;
    class Plugin;
    class DataConsumer;
    enum Type;
}

namespace gui
{
    class MainWindow;
}

class DataManager;

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
    * Adds the given plugin to the list of plugins kept by the core.
    */
    void addPlugin(plugin::Plugin* plugin);

    /**
    * Create a new dataset instance of the given kind.
    */
    const QString addData(const QString kind, const QString name) override;

    const QString createDerivedData(const QString kind, const QString name, const QString sourceName) override;

    /**
    * Create a new dataset from the given selection.
    */
    void createSubsetFromSelection(const Set& selection, const QString newSetName) override;

    /**
    * Request a plugin from the core by its name.
    */
    plugin::RawData& requestData(const QString name) override;

    /**
    * Request a dataset from the data manager by its name.
    */
    Set& requestSet(const QString name) override;

    /**
    * Requests the selection set belonging to the raw dataset with the given name.
    */
    Set& requestSelection(const QString name) override;

    /** Notify all data consumers that a new dataset has been added to the core. */
    void notifyDataAdded(const QString name);
    /** Notify all data consumers that a dataset has been changed. */
    void notifyDataChanged(const QString name);
    /** Notify all data consumers that a dataset has been removed. */
    void notifyDataRemoved(const QString name);
    /** Notify all data consumers that a selection has changed. */
    void notifySelectionChanged(const QString dataName);

    /**
    * Returns a reference to the main window for adding widgets to it.
    */
    gui::MainWindow& gui() const;
private:
    /** Checks if the given data consumer supports the kind data in the given set. */
    bool supportsSet(plugin::DataConsumer* dataConsumer, QString setName);
    /** Retrieves all data consumers from the plugin list. */
    std::vector<plugin::DataConsumer*> getDataConsumers();

    /** Destroys all plugins kept by the core */
    void destroyPlugins();

private:
    gui::MainWindow& _mainWindow;

    /** Plugin manager responsible for loading plugins and adding them to the core. */
    std::unique_ptr<plugin::PluginManager> _pluginManager;

    /** Data manager responsible for storing data sets and data selections */
    std::unique_ptr<DataManager> _dataManager;

    /** List of plugin instances currently present in the application. Instances are stored by type. */
    std::unordered_map<plugin::Type, std::vector<std::unique_ptr<plugin::Plugin>>> _plugins;
};

} // namespace hdps
