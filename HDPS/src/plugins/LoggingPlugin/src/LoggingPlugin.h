#pragma once

#include "LoggingWidget.h"

#include <ViewPlugin.h>

using namespace hdps::plugin;

/**
 * Logging view plugin
 *
 * This plugin provides a user interface inspecting HDPS logs
 *
 * @author Thomas Kroes
 */
class LoggingPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:
    LoggingPlugin(const PluginFactory* factory);
    
    void init() override;

private:
    LoggingWidget    _loggingWidget;      /** Loggin widget */
};

class LoggingPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "nl.BioVault.LoggingPlugin" FILE "LoggingPlugin.json")
    
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
