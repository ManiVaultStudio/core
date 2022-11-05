#pragma once

#include <ViewPlugin.h>

using namespace hdps::plugin;

/**
 * Data properties plugin
 *
 * This plugin provides a user interface for viewing/configuring a dataset and its associated actions.
 *
 * @author Thomas Kroes
 */
class DataPropertiesPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:
    DataPropertiesPlugin(const PluginFactory* factory);
    
    void init() override;
};

class DataPropertiesPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "nl.BioVault.DataPropertiesPlugin" FILE "DataPropertiesPlugin.json")
    
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
