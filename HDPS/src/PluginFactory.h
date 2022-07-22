#ifndef HDPS_PLUGINFACTORY_H
#define HDPS_PLUGINFACTORY_H

#include "PluginType.h"
#include "DataType.h"
#include "PluginProducerMetaData.h"
#include "Dataset.h"

#include <QObject>
#include <QIcon>
#include <QVariant>
#include <QAction>

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

    /**
     * Produces the plugin
     * @return Pointer to the produced plugin
     */
    virtual Plugin* produce() = 0;

    /**
     * Get a list of producer actions given a sequence of input dataset types
     * @param datasets Sequence of input datasets (order in which they were selected in the data hierarchy)
     * @return List of producer actions with which one (or more) plugins can be triggered
     */
    virtual QList<QAction*> getProducers(const Datasets& datasets) const {
        return QList<QAction*>();
    }

protected:

    /**
     * Get sequence of input datasets as a string list
     * @param datasets Sequence of input datasets (order in which they were selected in the data hierarchy)
     * @return String list of input dataset types
     */
    QStringList getDatasetTypes(const Datasets& datasets) const;

    /**
     * Determine whether each dataset in the sequence is of the same type
     * @param datasets Sequence of input datasets (order in which they were selected in the data hierarchy)
     * @param datasetType Type of input datasets to check for
     * @return Whether each dataset in the sequence is of the same type
     */
    bool areAllDatasetsOfTheSameType(const Datasets& datasets, const QString& datasetType) const;

private:
    QString     _kind;          /** Kind of plugin (e.g. scatter plot plugin & TSNE analysis plugin) */
    Type        _type;          /** Type of plugin (e.g. analysis, data, loader, writer & view) */
    QString     _guiName;       /** Name of the plugin in the GUI */
    QString     _version;       /** Plugin version */

protected:
    std::uint32_t   _numberOfInstances;     /** Number of plugin instances */

    friend class PluginManager;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::PluginFactory, "hdps.PluginFactory")

#endif // HDPS_PLUGINFACTORY_H
