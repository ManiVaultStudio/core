#pragma once

#include "TriggerAction.h"
#include "DecimalAction.h"

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
     * Get sha of plugin kind + trigger title
     * @return Sha
     */
    QString getSha() const;

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
     * Set input datasets
     * @param Vector of input datasets
     */
    void setDatasets(const Datasets& datasets);

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
    const QString       _sha;                   /** Cryptographic hash of the plugin kind and trigger title */
    const plugin::Type  _pluginType;            /** Type of plugin e.g. analysis, exporter */
    const QString       _pluginKind;            /** Kind of plugin */
    Datasets            _datasets;              /** Input datasets */
    WidgetAction*       _configurationAction;   /** Action for configuring the plugin creation */
};

using PluginTriggerActions = QVector<PluginTriggerAction*>;

}
}
