// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "DataHierarchyWidget.h"

#include <ViewPlugin.h>

using namespace mv::plugin;

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

    /**
     * Construct with pointer to plugin \p factory
     * @param factory Pointer to plugin factory
     */
    DataHierarchyPlugin(const PluginFactory* factory);

    /** Perform plugin initialization */
    void init() override;

private:
    DataHierarchyWidget     _dataHierarchyWidget;       /** Data hierarchy widget */
};

class DataHierarchyPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(mv::plugin::ViewPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "studio.manivault.DataHierarchyPlugin"
                      FILE  "DataHierarchyPlugin.json")
    
public:

    /** Constructor */
    DataHierarchyPluginFactory();

    /** Post-add initialization */
    void initialize() override;

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override;

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
