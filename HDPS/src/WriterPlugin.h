#ifndef HDPS_WRITERPLUGIN_H
#define HDPS_WRITERPLUGIN_H
/**
* Writer.h
*
* Base plugin class for plugins that write data to a file.
*/


#include "Plugin.h"

#include "util/DatasetRef.h"

#include <QString>

using namespace hdps::util;

namespace hdps
{
namespace plugin
{

class WriterPlugin : public Plugin
{
public:
    WriterPlugin(const PluginFactory* factory) :
        Plugin(factory),
        _input()
    {
    }

    ~WriterPlugin() override {};

    virtual void writeData() = 0;

public:

    /**
     * Set input dataset
     * @param inputDataset Reference to input dataset
     */
    void setInputDataset(DataSet& inputDataset) {
        _input.set(&inputDataset);
    }

    /** Get input dataset */
    template<typename DatasetType>
    DatasetType& getInputDataset() const {
        return dynamic_cast<DatasetType&>(*_input);
    }

protected:
    DatasetRef<DataSet>     _input;     /** Input dataset reference */
};


class WriterPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    WriterPluginFactory() :
        PluginFactory(Type::WRITER)
    {

    }
    ~WriterPluginFactory() override {};
    
    /** Returns the plugin icon */
    QIcon getIcon() const override {
        return Application::getIconFont("FontAwesome").getIcon("file-export");
    }

    WriterPlugin* produce() override = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::WriterPluginFactory, "hdps.WriterPluginFactory")

#endif // HDPS_WRITERPLUGIN_H
