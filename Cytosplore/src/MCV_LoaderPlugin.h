#ifndef MCV_LOADER_PLUGIN
#define MCV_LOADER_PLUGIN

#include "PluginFactory.h"

class MCV_LoaderPlugin : public Plugin
{
public:
    
    virtual ~MCV_LoaderPlugin() {};
};


class MCV_LoaderPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    
    virtual ~MCV_LoaderPluginFactory() {};
    
    virtual MCV_LoaderPlugin* produce() = 0;
};

Q_DECLARE_INTERFACE(MCV_LoaderPluginFactory, "cytosplore.MCV_LoaderPluginFactory")

#endif // MCV_LOADER_PLUGIN
