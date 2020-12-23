#ifndef HDPS_WRITERPLUGIN_H
#define HDPS_WRITERPLUGIN_H
/**
* Writer.h
*
* Base plugin class for plugins that write data to a file.
*/


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

    ~WriterPlugin() override {};

    /** Returns the icon of this plugin */
    QIcon getIcon() const override {
        return Application::getIconFont("FontAwesome").getIcon("file-export");
    }
};


class WriterPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    
    ~WriterPluginFactory() override {};
    
    WriterPlugin* produce() override = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::WriterPluginFactory, "cytosplore.WriterPluginFactory")

#endif // HDPS_WRITERPLUGIN_H
