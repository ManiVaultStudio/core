#pragma once

#include "PluginTriggerAction.h"
#include "ViewPlugin.h"

namespace hdps::gui {

/**
 * View plugin trigger action class
 *
 * Plugin trigger action extended with docking parameters for view plugins
 *
 * @author Thomas Kroes
 */
class ViewPluginTriggerAction : public PluginTriggerAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param viewPluginKind Kind of view plugin
     */
    ViewPluginTriggerAction(QObject* parent, const QString viewPluginKind);

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param viewPluginKind Kind of view plugin
     * @param dockViewPlugin Pointer to view plugin to which new view plugins are docked (new view plugins be docked top-level if nullptr)
     * @param dockArea Dock area in which new view plugins will be docked
     */
    ViewPluginTriggerAction(QObject* parent, const QString viewPluginKind, plugin::ViewPlugin* dockViewPlugin, gui::DockAreaFlag dockArea);

    /**
     * Get dock to view plugin
     * @return Pointer to view plugin to which new view plugins are docked (new view plugins be docked top-level if nullptr)
     */
    plugin::ViewPlugin* getDockToViewPlugin() const;

    /**
     * Set dock to view plugin
     * @param dockToViewPlugin Pointer to view plugin to which new view plugins are docked (new view plugins be docked top-level if nullptr)
     */
    void setDockToViewPlugin(plugin::ViewPlugin* dockToViewPlugin);

    /**
     * Get dock area
     * @return Dock area in which new view plugins will be docked
     */
    gui::DockAreaFlag getDockArea() const;

    /**
     * Set dock area
     * @param dockArea Dock area in which new view plugins will be docked
     */
    void setDockArea(gui::DockAreaFlag dockArea);

protected:

    /** Sets up the trigger action */
    virtual void initialize() override;

private:
    plugin::ViewPlugin*     _dockToViewPlugin;      /** Pointer to view plugin to which new view plugins are docked (new view plugins be docked top-level if nullptr) */
    gui::DockAreaFlag           _dockArea;              /** Dock area in which new view plugins will be docked */
};

using ViewPluginTriggerActions = QVector<ViewPluginTriggerAction*>;

}
