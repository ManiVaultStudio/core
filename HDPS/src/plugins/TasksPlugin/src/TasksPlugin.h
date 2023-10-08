// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <ViewPlugin.h>

#include <actions/TasksAction.h>

using namespace hdps;
using namespace hdps::plugin;
using namespace hdps::gui;

/**
 * Tasks plugin
 *
 * This plugin provides a user interface for viewing/configuring tasks.
 *
 * @author Thomas Kroes
 */
class TasksPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:

    /**
     * Construct with \p factory
     * @param factory Pointer to plugin factory which created this plugin
     */
    TasksPlugin(const PluginFactory* factory);
    
    void init() override;

private:
    
    
#ifdef _DEBUG

    /** Adds GUI for testing various actions (visible in debug mode only) */
    void addTestSuite();
#endif
    

private:
    TasksAction     _tasksAction;   /** Tasks action for displaying and interacting with the tasks in the system */
};

class TasksPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "NL.ManiVault.TasksPlugin" FILE "TasksPlugin.json")
    
public:

    /** Constructor */
    TasksPluginFactory();

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
