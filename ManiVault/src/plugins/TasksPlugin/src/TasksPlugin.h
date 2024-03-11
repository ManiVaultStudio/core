// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <ViewPlugin.h>

#include <actions/TasksTreeAction.h>

#include <models/TasksTreeModel.h>
#include <models/TasksFilterModel.h>

#ifdef _DEBUG
#define TEST_SUITE
#endif

//#define TEST_SUITE

using namespace mv;
using namespace mv::plugin;
using namespace mv::gui;

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

#ifdef TEST_SUITE

    /** Adds GUI for testing various actions (visible in debug mode only) */
    void addTestSuite();
#endif
    
private:
    TasksTreeModel      _model;         /** Tasks tree model */
    TasksFilterModel    _filterModel;   /** Filter model for the tasks model */
    TasksTreeAction     _tasksAction;   /** Tasks action for displaying and interacting with the tasks in the system */
};

class TasksPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(mv::plugin::ViewPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "studio.manivault.TasksPlugin"
                      FILE  "TasksPlugin.json")
    
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
