// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "DataPropertiesWidget.h"

#include <ViewPlugin.h>

#include <actions/TriggerAction.h>

using namespace mv::plugin;

/**
 * Data properties plugin
 *
 * This plugin provides a user interface for viewing/configuring a dataset and its associated actions.
 *
 * @author Thomas Kroes
 */
class DataPropertiesPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:
    DataPropertiesPlugin(const PluginFactory* factory);
    
    void init() override;

protected:

    /**
     * Updates the window title based on the \p selectedDataHierarchyItems in the data hierarchy
     * @param selectedDataHierarchyItems Items selected in the data hierarchy
     */
    void updateWindowTitle(DataHierarchyItems selectedDataHierarchyItems);

private:
    gui::TriggerAction      _additionalEditorAction;    /** Trigger action to start the data set editor */
    DataPropertiesWidget    _dataPropertiesWidget;      /** Data properties widget */

    friend class DataPropertiesWidget;
};

class DataPropertiesPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(mv::plugin::ViewPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "nl.BioVault.DataPropertiesPlugin" FILE "DataPropertiesPlugin.json")
    
public:

    /** Constructor */
    DataPropertiesPluginFactory();

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