#ifndef HDPS_PLUGIN_VIEW_PLUGIN
#define HDPS_PLUGIN_VIEW_PLUGIN

#include "widgets/DockableWidget.h"
#include "PluginFactory.h"
#include "DataConsumer.h"

namespace hdps {

namespace plugin {

class ViewPlugin : public gui::DockableWidget, public Plugin, public DataConsumer
{
    Q_OBJECT
    
public:
    ViewPlugin(QString name) : Plugin(Type::VIEW, name) { }
    virtual ~ViewPlugin() {};
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
