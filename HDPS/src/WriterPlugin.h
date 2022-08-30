#ifndef HDPS_WRITERPLUGIN_H
#define HDPS_WRITERPLUGIN_H
/**
* Writer.h
*
* Base plugin class for plugins that write data to a file.
*/


#include "Plugin.h"

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
     * Set input dataset smart pointer
     * @param inputDataset Smart pointer to input dataset
     */
    void setInputDataset(Dataset<DatasetImpl>& inputDataset) {
        _input = inputDataset;
    }

    /** Get input dataset smart pointer */
    template<typename DatasetType>
    Dataset<DatasetType>& getInputDataset() {
        return Dataset<DatasetType>(*_input);
    }

protected:
    Dataset<DatasetImpl>    _input;     /** Input dataset smart pointer */
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

    /**
     * Set name of the object
     * @param name Name of the object
     */
    void setObjectName(const QString& name)
    {
        QObject::setObjectName("Plugins/Writer/" + name);
    }

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override {
        return Application::getIconFont("FontAwesome").getIcon("file-export", color);
    }

    WriterPlugin* produce() override = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::WriterPluginFactory, "hdps.WriterPluginFactory")

#endif // HDPS_WRITERPLUGIN_H
