#ifndef HDPS_PLUGINFACTORY_H
#define HDPS_PLUGINFACTORY_H

#include "PluginType.h"
#include "DataType.h"

#include <QObject>

namespace hdps
{
    class DataSet;

namespace plugin
{

class Plugin;

class PluginFactory : public QObject
{
    Q_OBJECT

public:
    PluginFactory(Type type) :
        _kind(),
        _type(type),
        _guiName()
    {
    }

    ~PluginFactory() override {};

    void setKind(QString kind) { _kind = kind; }
    QString getKind() const { return _kind; }

    Type getType() const { return _type; }

    /** Get the menu name of the plugin */
    QString getGuiName() const {
        return _guiName;
    }

    /**
     * Set the GUI name of the plugin
     * @param guiName GUI name of the plugin
     */
    void setGuiName(const QString& guiName) {
        _guiName = guiName;
    }

    virtual Plugin* produce() = 0;

    /**
     * Returns a list of datatypes that are supported for operations by
     * the plugin produced in this factory.
     */
    virtual DataTypes supportedDataTypes() const = 0;

    //virtual bool isCompatible(DataSet& dataSet) = 0;

private:
    QString     _kind;      /** Kind of plugin (e.g. scatter plot plugin & TSNE analysis plugin) */
    Type        _type;      /** Type of plugin (e.g. analysis, data, loader, writer & view) */
    QString     _guiName;   /** Name of the plugin in the GUI */
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::PluginFactory, "hdps.PluginFactory")

#endif // HDPS_PLUGINFACTORY_H
