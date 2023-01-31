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
 * Action class for triggering the creation of plugins.
 * 
 * By default, a plugin is automatically created when the trigger action is triggered.
 * Custom plugin creation is possible by setting a creation handler function, see setCreatePluginFunction()
 *
 * @author Thomas Kroes
 */
class PluginTriggerAction : public TriggerAction
{
    Q_OBJECT
    
        /** Request plugin callback function (invoked when the trigger action is triggered) */
    using RequestPluginCallback = std::function<void(PluginTriggerAction&)>;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param pluginFactory Pointer to plugin factory
     * @param title Title of the plugin trigger action
     * @param icon Icon
     * @param tooltip Tooltip of the plugin trigger action
     */
    PluginTriggerAction(QObject* parent, const plugin::PluginFactory* pluginFactory, const QString& title, const QString& tooltip, const QIcon& icon);

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param pluginFactory Pointer to plugin factory
     * @param title Title of the plugin trigger action
     * @param tooltip Tooltip of the plugin trigger action
     * @param icon Icon
     * @param requestPluginCallback Callback which is invoked when the trigger action is triggered
     */
    PluginTriggerAction(QObject* parent, const plugin::PluginFactory* pluginFactory, const QString& title, const QString& tooltip, const QIcon& icon, RequestPluginCallback requestPluginCallback);

    /**
     * Copy constructor
     * @param pluginTriggerAction Reference to other plugin trigger action
     */
    PluginTriggerAction(const PluginTriggerAction& pluginTriggerAction);

    /**
     * Get the plugin factory
     * @return Pointer to the plugin factory
     */
    const plugin::PluginFactory* getPluginFactory() const;

    /**
     * Get location of the plugin trigger action
     * @return Location of the plugin trigger action
     */
    QString getLocation() const;

    /**
     * Set location of the plugin trigger action
     * @param location Location of the plugin trigger action
     */
    void setLocation(const QString& location);

    /**
     * Get sha of plugin kind + trigger title
     * @return Sha
     */
    QString getSha() const;

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

    /**
     * Override base class to also update location
     * @param text Action text
     */
    void setText(const QString& text);

    /**
     * Set the callback function which is invoked when the trigger action is triggered
     * @param requestPluginCallback Request plugin callback function (invoked when the trigger action is triggered)
     */
    void setRequestPluginCallback(RequestPluginCallback requestPluginCallback);

protected:

    /** Sets up the trigger action */
    virtual void initialize();

private:

    /** Request a plugin by calling the request plugin callback (invoked when the trigger action is triggered) */
    void requestPlugin();

private:
    const plugin::PluginFactory*    _pluginFactory;             /** Pointer to plugin factory */
    QString                         _location;                  /** Determines where the plugin trigger action resides w.r.t. other plugin trigger actions (for instance in the data hierarchy context menu) in a path like fashion e.g. import/images */
    QString                         _sha;                       /** Cryptographic hash of the plugin kind and trigger title */
    WidgetAction*                   _configurationAction;       /** Action for configuring the plugin creation */
    RequestPluginCallback           _requestPluginCallback;     /** Request plugin callback function which should create the plugin (invoked when the trigger action is triggered) */

    friend class plugin::PluginFactory;
};

using PluginTriggerActions = QVector<QPointer<PluginTriggerAction>>;

}
