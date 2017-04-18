#ifndef MCV_DATA_TYPE_PLUGIN
#define MCV_DATA_TYPE_PLUGIN

#include "PluginFactory.h"

class MCV_DataTypePlugin : public Plugin
{
public:
    
    virtual ~MCV_DataTypePlugin() {};
};


class MCV_DataTypePluginFactory : public QObject
{
    Q_OBJECT
    
public:
    
    virtual ~MCV_DataTypePluginFactory() {};
    
    virtual MCV_DataTypePlugin* produce() = 0;
};

Q_DECLARE_INTERFACE(MCV_DataTypePluginFactory, "cytosplore.MCV_DataTypePluginFactory")

#endif // MCV_DATA_TYPE_PLUGIN
