#pragma once

#include "TriggerAction.h"

#include "PluginType.h"
#include "Dataset.h"

namespace hdps {

namespace gui {

/**
 * Plugin trigger action class
 *
 * Action class for triggering the creation of plugins (and possible configuration)
 *
 * @author Thomas Kroes
 */
class PluginTriggerAction : public TriggerAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Plugin trigger title
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @param pluginKind Kind of plugin
     * @param datasets Input datasets
     */
    PluginTriggerAction(QObject* parent, const QString& title, const plugin::Type& pluginType, const QString& pluginKind, const Datasets& datasets);

    /**
     * Get hash
     * @return Cryptographic hash of the plugin kind and trigger title
     */
    QString getHash() const;

    /**
     * Get type of plugin
     * @return Type of plugin
     */
    plugin::Type getPluginType() const;

    /**
     * Get kind of plugin
     * @return Kind of plugin in string format
     */
    QString getPluginKind() const;

    /**
     * Get input datasets
     * @return Vector of input datasets
     */
    Datasets getDatasets() const;

    /**
     * Get configuration action
     * @return Action for configuring the plugin creation (if available)
     */
    WidgetAction* getConfigurationAction();

    /**
     * Set configuration action
     * @param configurationAction Pointer to action for configuring the plugin creation
     */
    void setConfigurationAction(WidgetAction* configurationAction);

private:
    const QString       _hash;                  /** Cryptographic hash of the plugin kind and trigger title */
    const plugin::Type  _pluginType;            /** Type of plugin e.g. analysis, exporter */
    const QString       _pluginKind;            /** Kind of plugin */
    const Datasets      _datasets;              /** Input datasets */
    WidgetAction*       _configurationAction;   /** Action for configuring the plugin creation */
};

using PluginTriggerActions = QVector<PluginTriggerAction*>;

}
}
