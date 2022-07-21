#ifndef HDPS_PLUGINFACTORY_H
#define HDPS_PLUGINFACTORY_H

#include "PluginType.h"
#include "DataType.h"
#include "PluginProducerMetaData.h"

#include <QObject>
#include <QIcon>
#include <QVariant>
#include <QRegularExpression>

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

public: // Producer(s) with which the plugin may be created

    /** Get the plugin producers(s) */
    QVariant getProducers() const {
        return _producers;
    }

    /**
     * Set the plugin producers(s)
     * @param producers Plugin producers
     */
    void setProducers(const QVariant& producers) {
        _producers = producers;
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
     * Get a list of producer meta data given a sequence of input dataset types
     * @param datasetTypes Sequence of input dataset types
     * @return List of producer meta data with which the user interface can be configured (menu's etc)
     */
    virtual PluginProducersMetaData getProducers(const QStringList& datasetTypes) const final {
        PluginProducersMetaData producersMetaData;

        QRegularExpression re;

        for (auto input : _producers.toList()) {
            const auto inputMap = input.toMap();

            re.setPattern(inputMap["re"].toString());

            if (!re.match(datasetTypes.join(", ")).hasMatch())
                continue;

            producersMetaData << PluginProducerMetaData(_type, _kind, datasetTypes, inputMap["title"].toString(), inputMap["description"].toString());
        }

        return producersMetaData;
    }

private:
    QString     _kind;          /** Kind of plugin (e.g. scatter plot plugin & TSNE analysis plugin) */
    Type        _type;          /** Type of plugin (e.g. analysis, data, loader, writer & view) */
    QString     _guiName;       /** Name of the plugin in the GUI */
    QString     _version;       /** Plugin version */
    QVariant    _producers;     /** Available producers */

protected:
    std::uint32_t   _numberOfInstances;     /** Number of plugin instances */

    friend class PluginManager;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::PluginFactory, "hdps.PluginFactory")

#endif // HDPS_PLUGINFACTORY_H
