#pragma once

#include <ViewPlugin.h>

using namespace hdps::plugin;

/**
 * Public actions  plugin
 *
 * This plugin provides a user interface for viewing/configuring public actions.
 *
 * @author Thomas Kroes
 */
class PublicActionsPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:
    PublicActionsPlugin(const PluginFactory* factory);
    
    void init() override;
};

class PublicActionsPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "nl.BioVault.PublicActionsPlugin" FILE "PublicActionsPlugin.json")
    
public:

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
