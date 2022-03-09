#ifndef HDPS_PLUGINFACTORY_H
#define HDPS_PLUGINFACTORY_H

#include "PluginType.h"
#include "DataType.h"

#include <QObject>
#include <QIcon>

namespace hdps
{
    class DatasetImpl;

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
        _guiName(),
        _version(),
        _numberOfInstances(0)
    {
    }

    ~PluginFactory() override {};

    void setKind(QString kind) { _kind = kind; }
    QString getKind() const { return _kind; }

    Type getType() const { return _type; }

public: // GUI name

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

public: // Version

    /** Get the plugin version */
    QString getVersion() const {
        return _version;
    }

    /**
     * Set the plugin version
     * @param version Plugin version
     */
    void setVersion(const QString& version) {
        _version = version;
    }

public:

    /** Returns the plugin icon */
    virtual QIcon getIcon() const = 0;

    virtual Plugin* produce() = 0;

    /**
     * Returns a list of datatypes that are supported for operations by
     * the plugin produced in this factory.
     */
    virtual DataTypes supportedDataTypes() const = 0;

    //virtual bool isCompatible(DataSet& dataSet) = 0;

private:
    QString         _kind;                  /** Kind of plugin (e.g. scatter plot plugin & TSNE analysis plugin) */
    Type            _type;                  /** Type of plugin (e.g. analysis, data, loader, writer & view) */
    QString         _guiName;               /** Name of the plugin in the GUI */
    QString         _version;               /** Plugin version */

protected:
    std::uint32_t   _numberOfInstances;     /** Number of plugin instances */

    friend class PluginManager;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::PluginFactory, "hdps.PluginFactory")

#endif // HDPS_PLUGINFACTORY_H
