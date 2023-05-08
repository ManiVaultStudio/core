#pragma once

#include "PublicActionsModel.h"

#include "widgets/ActionsWidget.h"

#include <ViewPlugin.h>

using namespace hdps;
using namespace hdps::plugin;
using namespace hdps::gui;

/**
 * Shared parameters plugin
 *
 * This plugin provides a user interface for viewing/configuring shared parameters.
 *
 * @author Thomas Kroes
 */
class SharedParametersPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:
    SharedParametersPlugin(const PluginFactory* factory);
    
    void init() override;

private:
    PublicActionsModel      _publicActionsModel;    /** Public actions model of the top-level public actions and their descendants */
    ActionsWidget           _actionsWidget;         /** Widget for interaction with shared parameters */
    gui::ToggleAction       _expertModeAction;      /** In expert mode, all descendants of a root public action are displayed, otherwise they are hidden */
};

class ActionsPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "NL.ManiVault.SharedParametersPlugin" FILE "SharedParametersPlugin.json")
    
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
