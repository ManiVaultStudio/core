#pragma once

#include "PluginTriggerAction.h"

#include <actions/OptionAction.h>
#include <actions/WidgetActionCollapsedWidget.h>

namespace hdps::gui {

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
     * @param title Plugin trigger picker title
     */
    PluginTriggerPickerAction(QObject* parent, const QString& title = "");

    /**
     * Establishes the plugin trigger actions for \p pluginType and \p datasets
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @param datasets Input datasets
     */
    void initialize(const plugin::Type& pluginType, const Datasets& datasets);

    /**
     * Establishes the plugin trigger actions for \p pluginType and \p dataTypes
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @param dataTypes Vector of dataTypes
     */
    void initialize(const plugin::Type& pluginType, const DataTypes& dataTypes);

    /**
     * Establishes the plugin trigger actions for \p pluginKind and \p datasets
     * @param pluginKind Kind of plugin
     * @param datasets Input datasets
     */
    void initialize(const QString& pluginKind, const Datasets& datasets);

    /**
     * Establishes the plugin trigger actions for \p pluginKind and \p dataTypes
     * @param pluginKind Kind of plugin
     * @param dataTypes Vector of dataTypes
     */
    void initialize(const QString& pluginKind, const DataTypes& dataTypes);

    /**
     * Get plugin trigger action by its SHA
     * @return sha SHA of the plugin trigger action to find
     * @return Pointer to plugin trigger action (if found)
     */
    QPointer<PluginTriggerAction> getPluginTriggerAction(const QString& sha);

    /**
     * Get plugin trigger actions
     * @return List of plugin trigger actions
     */
    PluginTriggerActions getPluginTriggerActions();

    /**
     * Get current plugin trigger action (if any)
     * @return Pointer to current plugin trigger action (if any)
     */
    QPointer<PluginTriggerAction> getCurrentPluginTriggerAction();

    /**
     * Set current plugin trigger action by \p pluginTriggerAction
     * @param pluginTriggerAction Pointer to plugin trigger action
     */
    void setCurrentPluginTriggerAction(QPointer<PluginTriggerAction> pluginTriggerAction);

    /**
     * Set current plugin trigger action by \p sha
     * @param sha Plugin trigger action sha
     */
    void setCurrentPluginTriggerAction(const QString& sha);

    /**
     * Get trigger selection option action
     * @return Reference to trigger selection option action
     */
    OptionAction& getSelectTriggerAction();

signals:

    /**
     * Signals that the current plugin trigger action changed
     * @param currentPluginTriggerAction Pointer to current plugin trigger action that changed
     */
    void currentPluginTriggerActionChanged(const QPointer<PluginTriggerAction> currentPluginTriggerAction);

    /**
     * Signals that the plugin trigger actions have changed
     * @param pluginTriggerActions List of plugin trigger actions
     */
    void pluginTriggerActionsChanged(const PluginTriggerActions& pluginTriggerActions);

private:
    PluginTriggerActions    _pluginTriggerActions;      /** Plugin trigger actions */
    OptionAction            _selectTriggerAction;       /** Select trigger action */
};

}

Q_DECLARE_METATYPE(hdps::gui::PluginTriggerPickerAction)

inline const auto pluginTriggerPickerActionMetaTypeId = qRegisterMetaType<hdps::gui::PluginTriggerPickerAction*>("PluginTriggerPickerAction");
