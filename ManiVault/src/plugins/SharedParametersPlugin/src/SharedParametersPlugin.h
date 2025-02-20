// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PublicActionsModel.h"

#include "widgets/ActionsWidget.h"

#include <ViewPlugin.h>

using namespace mv;
using namespace mv::plugin;
using namespace mv::gui;

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

    /**
     * Construct with pointer to plugin \p factory
     * @param factory Pointer to plugin factory
     */
    SharedParametersPlugin(const PluginFactory* factory);

    /** Perform plugin initialization */
    void init() override;

private:
    PublicActionsModel  _publicActionsModel;    /** Public actions model of the top-level public actions and their descendants */
    ActionsWidget       _actionsWidget;         /** Widget for interaction with shared parameters */
};

class SharedParametersPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(mv::plugin::ViewPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "studio.manivault.SharedParametersPlugin"
                      FILE  "SharedParametersPlugin.json")
    
public:

    /** Constructor */
    SharedParametersPluginFactory();

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
    ViewPlugin* produce() override;
};
