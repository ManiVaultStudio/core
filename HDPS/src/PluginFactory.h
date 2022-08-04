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

    namespace gui
    {
        class PluginTriggerAction;
    }

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

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    virtual QIcon getIcon(const QColor& color = Qt::black) const = 0;

    /**
     * Produces the plugin
     * @return Pointer to the produced plugin
     */
    virtual Plugin* produce() = 0;

    /**
     * Get a list of plugin trigger actions given a vector of input datasets
     * @param datasets Vector of input datasets
     * @return List of plugin trigger actions
     */
    virtual QList<gui::PluginTriggerAction*> getPluginTriggerActions(const Datasets& datasets) const {
        return QList<gui::PluginTriggerAction*>();
    }

protected:

    /**
     * Get sequence of input datasets as a string list
     * @param datasets Sequence of input datasets (order in which they were selected in the data hierarchy)
     * @return String list of input dataset types
     */
    static QStringList getDatasetTypesAsStringList(const Datasets& datasets);

    /**
     * Determine whether each dataset in the sequence is of the same type
     * @param datasets Sequence of input datasets (order in which they were selected in the data hierarchy)
     * @param datasetType Type of input datasets to check for
     * @return Whether each dataset in the sequence is of the same type
     */
    static bool areAllDatasetsOfTheSameType(const Datasets& datasets, const QString& datasetType);

    /**
     * Get number of datasets for \p datasetType
     * @param datasetType Dataset type as string
     * @return Number of datasets of \p datasetType
     */
    static std::uint16_t getNumberOfDatasetsForType(const Datasets& datasets, const QString& datasetType);

    /**
     * Convenience function for generating a plugin trigger action (icon from the plugin factory)
     * @param title Title of the plugin trigger action
     * @param description Description of the plugin trigger action
     * @param datasets Input datasets
     * @return Pointer to plugin trigger action
     */
    gui::PluginTriggerAction* createPluginTriggerAction(const QString& title, const QString& description, const Datasets& datasets) const;

    /**
     * Convenience function for generating a plugin trigger action
     * @param title Title of the plugin trigger action
     * @param description Description of the plugin trigger action
     * @param datasets Input datasets
     * @param iconName Name of the icon
     * @return Pointer to plugin trigger action
     */
    gui::PluginTriggerAction* createPluginTriggerAction(const QString& title, const QString& description, const Datasets& datasets, const QString& iconName) const;

    /**
     * Convenience function for generating a plugin trigger action
     * @param title Title of the plugin trigger action
     * @param description Description of the plugin trigger action
     * @param datasets Input datasets
     * @param icon Icon
     * @return Pointer to plugin trigger action
     */
    gui::PluginTriggerAction* createPluginTriggerAction(const QString& title, const QString& description, const Datasets& datasets, const QIcon& icon) const;

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
