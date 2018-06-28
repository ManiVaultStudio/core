#pragma once

#include "PluginFactory.h"
#include "Set.h"

namespace hdps {

namespace plugin {

class RawData : public Plugin
{
public:
    RawData(QString name) : Plugin(Type::DATA, name) { }
    virtual ~RawData() {};

    virtual Set* createSet() const = 0;

    bool isDerivedData() const
    {
        return derived;
    }

    QString getSourceData() const
    {
        return sourceDataName;
    }

    Set& getSelection() const
    {
        return isDerivedData() ? _core->requestSelection(sourceDataName) : _core->requestSelection(getName());
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


class RawDataFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    
    virtual ~RawDataFactory() {};
    
    virtual RawData* produce() = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::RawDataFactory, "hdps.RawDataFactory")
