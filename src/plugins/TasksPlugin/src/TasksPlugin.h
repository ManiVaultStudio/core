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

/**
 * Tasks plugin
 *
 * This plugin provides a user interface for viewing/configuring tasks.
 *
 * @author Thomas Kroes
 */
class TasksPlugin : public mv::plugin::ViewPlugin
{
    Q_OBJECT
    
public:

    /**
     * Construct with pointer to plugin \p factory
     * @param factory Pointer to plugin factory
     */
    TasksPlugin(const mv::plugin::PluginFactory* factory);

    /** Perform plugin initialization */
    void init() override;

private:

#ifdef TEST_SUITE

    /** Adds GUI for testing various actions (visible in debug mode only) */
    void addTestSuite();
#endif
    
private:
    mv::TasksTreeModel           _model;         /** Tasks tree model */
    mv::TasksFilterModel         _filterModel;   /** Filter model for the tasks model */
    mv::gui::TasksTreeAction     _tasksAction;   /** Tasks action for displaying and interacting with the tasks in the system */
};

class TasksPluginFactory : public mv::plugin::ViewPluginFactory
{
    Q_INTERFACES(mv::plugin::ViewPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "studio.manivault.TasksPlugin"
                      FILE  "PluginInfo.json")
    
public:

    /** Constructor */
    TasksPluginFactory();

    /**
     * Get the read me markdown file URL
     * @return URL of the read me markdown file
     */
    QUrl getReadmeMarkdownUrl() const override;

    /**
     * Get the URL of the GitHub repository
     * @return URL of the GitHub repository
     */
    QUrl getRepositoryUrl() const override;

    /**
     * Produces the plugin
     * @return Pointer to the produced plugin
     */
    mv::plugin::ViewPlugin* produce() override;
};
