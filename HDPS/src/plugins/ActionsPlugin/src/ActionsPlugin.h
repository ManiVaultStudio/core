#pragma once

#include "ActionsWidget.h"

#include <ViewPlugin.h>

using namespace hdps::plugin;

/**
 * Actions  plugin
 *
 * This plugin provides a user interface for viewing/configuring (public) actions.
 *
 * @author Thomas Kroes
 */
class ActionsPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:
    ActionsPlugin(const PluginFactory* factory);
    
    void init() override;

private:
    ActionsWidget     _actionsWidget;       /** Widget for interaction with (public) actions */
};

class ActionsPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "nl.BioVault.ActionsPlugin" FILE "ActionsPlugin.json")
    
public:

    /** Constructor */
    ActionsPluginFactory();

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override;
    
	/**
     * Produces the plugin
     * @return Pointer to the produced plugin
     */
    ViewPlugin* produce() override;
};
