#pragma once

#include <ViewPlugin.h>

using namespace hdps::plugin;

/**
 * Data hierarchy view plugin
 *
 * This plugin visualizes the data hierarchy and allows users the interact with it.
 *
 * @author Thomas Kroes
 */
class DataHierarchyPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:
    DataHierarchyPlugin(const PluginFactory* factory);
    
    void init() override;
};

class DataHierarchyPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "nl.BioVault.DataHierarchyPlugin" FILE "DataHierarchyPlugin.json")
    
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
