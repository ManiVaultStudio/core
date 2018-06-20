#pragma once

#include "PluginFactory.h"

#include <QString>

namespace hdps {

namespace plugin {

class LoaderPlugin : public Plugin
{
public:
    LoaderPlugin(QString name) : Plugin(Type::LOADER, name) { }

    /**
     * Should be implemented by loader plugins to parse their specific data.
     * This function will be called when the user clicks on the menu item for this loader.
     * The implementation is free to create file dialogs if desired.
     */
    virtual bool loadData() = 0;

    virtual ~LoaderPlugin() {};
};


class LoaderPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    
    virtual ~LoaderPluginFactory() {};
    
    /**
    * Produces an instance of a loader plugin. This function gets called by the plugin manager.
    */
    virtual LoaderPlugin* produce() = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::LoaderPluginFactory, "cytosplore.LoaderPluginFactory")
