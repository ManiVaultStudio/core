#ifndef HDPS_PLUGIN_DATA_TYPE_PLUGIN
#define HDPS_PLUGIN_DATA_TYPE_PLUGIN

#include "PluginFactory.h"
#include "Set.h"

namespace hdps {

namespace plugin {

class DataTypePlugin : public Plugin
{
public:
    DataTypePlugin(QString name) : Plugin(Type::DATA_TYPE, name) { }
    virtual ~DataTypePlugin() {};

    virtual Set* createSet() const = 0;

    bool isDerivedData() const
    {
        return derived;
    }

    QString getSourceData() const
    {
        return sourceDataName;
    }

    void setDerived(QString sourceData)
    {
        derived = true;
        sourceDataName = sourceData;
    }

protected:
    bool derived = false;

    QString sourceDataName;
};


class DataTypePluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    
    virtual ~DataTypePluginFactory() {};
    
    virtual DataTypePlugin* produce() = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::DataTypePluginFactory, "cytosplore.DataTypePluginFactory")

#endif // HDPS_PLUGIN_DATA_TYPE_PLUGIN
