#pragma once

#include "PluginFactory.h"

#include <QString>

#include <exception>

namespace hdps {

namespace plugin {

struct DataLoadException : public std::exception
{
public:
    DataLoadException(QString filePath, QString reason)
        : _filePath(filePath),
        _reason(reason)
    {
    }

    const char* what() const throw () override
    {
        return ("Failed to load file at: " + _filePath + "\nReason: " + _reason).toStdString().c_str();
    }

private:
    QString _filePath;
    QString _reason;
};


class LoaderPlugin : public Plugin
{
public:
    LoaderPlugin(QString name) : Plugin(Type::LOADER, name) { }

    /**
     * Should be implemented by loader plugins to parse their specific data.
     * This function will be called when the user clicks on the menu item for this loader.
     * The implementation is free to create file dialogs if desired.
     */
    virtual void loadData() = 0;

    ~LoaderPlugin() override {};
};


class LoaderPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    
    ~LoaderPluginFactory() override {};
    
    /**
    * Produces an instance of a loader plugin. This function gets called by the plugin manager.
    */
    LoaderPlugin* produce() override = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::LoaderPluginFactory, "cytosplore.LoaderPluginFactory")
