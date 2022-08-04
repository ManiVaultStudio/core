#pragma once

#include "PluginTriggerAction.h"

#include <actions/OptionAction.h>
#include <actions/WidgetActionCollapsedWidget.h>

namespace hdps {

namespace gui {

/**
 * Plugin trigger picker action class
 *
 * Action class for picking (and configuring) a plugin trigger action
 *
 * @author Thomas Kroes
 */
class PluginTriggerPickerAction : public TriggerAction
{
    Q_OBJECT

public:

    /** Widget plugin trigger picker action */
    class Widget : public WidgetActionWidget {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param pluginTriggerPickerAction Pointer to plugin trigger picker action
         * @param widgetFlags Widget flags
         */
        Widget(QWidget* parent, PluginTriggerPickerAction* pluginTriggerPickerAction, const std::int32_t& widgetFlags);

    protected:
        PluginTriggerPickerAction*      _pluginTriggerPickerAction;     /** Pointer to plugin trigger picker action */
        OptionAction                    _selectTriggerAction;           /** Select trigger action */
        WidgetActionCollapsedWidget     _configurationToolButton;       /** Tool button which opens the configuration widget */

        friend class PluginTriggerPickerAction;
    };

protected:

    /**
     * Get widget representation of the plugin trigger picker action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    PluginTriggerPickerAction(QObject* parent);

    /**
     * Initializes the plugin trigger actions
     * @param title Plugin trigger picker title
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @param datasets Input datasets
     */
    void initialize(const QString& title, const plugin::Type& pluginType, const Datasets& datasets);

    /**
     * Initializes the plugin trigger actions
     * @param title Plugin trigger picker title
     * @param pluginKind Kind of plugin
     * @param datasets Input datasets
     */
    void initialize(const QString& title, const QString& pluginKind, const Datasets& datasets);

    /**
     * Get plugin trigger actions
     * @return List of plugin trigger actions
     */
    QList<PluginTriggerAction*> getPluginTriggerActions();

signals:

    /**
     * Signals that the plugin trigger actions have changed
     * @param pluginTriggerActions List of plugin trigger actions
     */
    void pluginTriggerActionsChanged(const QList<PluginTriggerAction*>& pluginTriggerActions);

private:
    Datasets                        _datasets;                  /** Input datasets */
    QList<PluginTriggerAction*>     _pluginTriggerActions;      /** Plugin trigger actions */
};

}
}
