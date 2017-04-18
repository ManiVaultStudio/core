#ifndef MCV_TRANSFORMATION_PLUGIN
#define MCV_TRANSFORMATION_PLUGIN

#include "PluginFactory.h"

class MCV_TransformationPlugin : public Plugin
{
public:
    
    virtual ~MCV_TransformationPlugin() {};
};


class MCV_TransformationPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    
    virtual ~MCV_TransformationPluginFactory() {};
    
    virtual MCV_TransformationPlugin* produce() = 0;
};

Q_DECLARE_INTERFACE(MCV_TransformationPluginFactory, "cytosplore.MCV_TransformationPluginFactory")

#endif // MCV_TRANSFORMATION_PLUGIN
