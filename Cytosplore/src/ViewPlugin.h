#ifndef HDPS_PLUGIN_VIEW_PLUGIN
#define HDPS_PLUGIN_VIEW_PLUGIN

#include "DockableWidget.h"
#include "PluginFactory.h"

namespace hdps {

namespace plugin {

class ViewPlugin : public gui::DockableWidget, public Plugin
{
    Q_OBJECT
    
public:
    ViewPlugin(QString name) : Plugin(name) { }
    virtual ~ViewPlugin() {};
    
    virtual void dataAdded() = 0;
    virtual void dataRefreshed() = 0;
};

class ViewPluginFactory : public PluginFactory
{
    Q_OBJECT

public:

    virtual ~ViewPluginFactory() {};

    virtual ViewPlugin* produce() = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::ViewPluginFactory, "cytosplore.ViewPluginFactory")

#endif // HDPS_PLUGIN_VIEW_PLUGIN
