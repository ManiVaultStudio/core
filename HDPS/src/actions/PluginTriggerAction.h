#pragma once

#include "TriggerAction.h"
#include "DecimalAction.h"

#include "PluginType.h"
#include "Dataset.h"

namespace hdps::plugin {
    class PluginFactory;
}

namespace hdps::gui {

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
     * @param pluginKind Kind of plugin
     * @param title Plugin trigger title (if title is in path format, the trigger will be added to the data hierarchy context menu in a hierarchical fashion)
     * @param datasets Input datasets
     */
    PluginTriggerAction(QObject* parent, const QString pluginKind, const QString& title, const Datasets& datasets = Datasets());

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param pluginFactory Pointer to plugin factory
     * @param title Plugin trigger title (if title is in path format, the trigger will be added to the data hierarchy context menu in a hierarchical fashion)
     * @param datasets Input datasets
     */
    PluginTriggerAction(QObject* parent, const plugin::PluginFactory* pluginFactory, const QString& title, const Datasets& datasets = Datasets());

    /**
     * Get the plugin factory
     * @return Pointer to the plugin factory
     */
    const plugin::PluginFactory* getPluginFactory() const;

    /**
     * Get title
     * @return Title
     */
    QString getTitle() const;

    /**
     * Set title
     * @param title Title
     */
    void setTitle(const QString& title);

    /**
     * Get sha of plugin kind + trigger title
     * @return Sha
     */
    QString getSha() const;

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

protected:

    /** Sets up the trigger action */
    virtual void initialize();

signals:

    /**
     * Signals that a plugin is produced
     * @param plugin Pointer to produced plugin
     */
    void pluginProduced(plugin::Plugin* plugin);

private:
    const QString                   _pluginKind;            /** Kind of plugin */
    const plugin::PluginFactory*    _pluginFactory;         /** Pointer to plugin factory */
    QString                         _title;                 /** Plugin trigger title (if title is in path format, the trigger will be added to the data hierarchy context menu in a hierarchical fashion) */
    QString                         _sha;                   /** Cryptographic hash of the plugin kind and trigger title */
    Datasets                        _datasets;              /** Input datasets */
    WidgetAction*                   _configurationAction;   /** Action for configuring the plugin creation */

    friend class plugin::PluginFactory;
};

using PluginTriggerActions = QVector<PluginTriggerAction*>;

}
