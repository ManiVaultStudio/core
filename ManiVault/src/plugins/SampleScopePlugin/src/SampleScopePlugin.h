// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "SampleScopeWidget.h"

#include <ViewPlugin.h>

#include <actions/HorizontalGroupAction.h>
#include <actions/PluginPickerAction.h>
#include <actions/ViewPluginSamplerAction.h>

/**
 * Sample scope plugin
 *
 * This plugin provides a user interface for exploring the sample context of another view.
 *
 * @author Thomas Kroes
 */
class SampleScopePlugin : public mv::plugin::ViewPlugin
{
    Q_OBJECT
    
public:

    /**
     * Construct with pointer to plugin \p factory
     * @param factory Pointer to plugin factory
     */
    SampleScopePlugin(const mv::plugin::PluginFactory* factory);

    /** Perform plugin initialization */
    void init() override;

private:
    SampleScopeWidget                   _sampleScopeWidget;             /** Sample scope widget */
    mv::gui::HorizontalGroupAction      _horizontalGroupAction;         /** Horizontal group action for settings */
    mv::gui::PluginPickerAction         _sourcePluginPickerAction;      /** Action for picking the source plugin */
    mv::gui::ViewPluginSamplerAction*   _viewPluginSamplerAction;       /** Pointer to current view plugin sampler action */
};

class SampleScopePluginFactory : public mv::plugin::ViewPluginFactory
{
    Q_INTERFACES(mv::plugin::ViewPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "studio.manivault.SampleScopePlugin"
                      FILE  "SampleScopePlugin.json")
    
public:

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override;

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