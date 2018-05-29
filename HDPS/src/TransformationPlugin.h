#pragma once

#include "PluginFactory.h"

namespace hdps
{
namespace plugin
{

class TransformationPlugin : public Plugin
{
public:
    TransformationPlugin(QString name) : Plugin(Type::TRANFORMATION, name) { }
    virtual ~TransformationPlugin() {};
};


class TransformationPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    
    virtual ~TransformationPluginFactory() {};
    
    /**
    * Produces an instance of a transformation plugin. This function gets called by the plugin manager.
    */
    virtual TransformationPlugin* produce() = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::TransformationPluginFactory, "cytosplore.TransformationPluginFactory")
