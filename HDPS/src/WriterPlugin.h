/**
* Writer.h
*
* Base plugin class for plugins that write data to a file.
*/

#pragma once

#include "PluginFactory.h"

#include <QString>

namespace hdps
{
namespace plugin
{

class WriterPlugin : public Plugin
{
public:
    WriterPlugin(QString name) : Plugin(Type::WRITER, name) { }

    virtual void writeData() = 0;

    virtual ~WriterPlugin() {};
};


class WriterPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    
    virtual ~WriterPluginFactory() {};
    
    virtual WriterPlugin* produce() = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::WriterPluginFactory, "cytosplore.WriterPluginFactory")
