// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "LoggingWidget.h"

#include <ViewPlugin.h>

using namespace mv::plugin;

/**
 * Logging view plugin
 *
 * This plugin provides a user interface for inspecting ManiVault logs
 *
 * @author Thomas Kroes
 */
class LoggingPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:

    /**
     * Constructor
     * @param factory Pointer to plugin factory
     */
    LoggingPlugin(const PluginFactory* factory);
    
    /** Perform plugin initialization */
    void init() override;

private:
    LoggingWidget    _loggingWidget;      /** Logging widget */
};

/**
 * Logging view plugin factory
 *
 * @author Thomas Kroes
 */
class LoggingPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(mv::plugin::ViewPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "nl.BioVault.LoggingPlugin" FILE "LoggingPlugin.json")
    
public:

    /** Constructor */
    LoggingPluginFactory();

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
