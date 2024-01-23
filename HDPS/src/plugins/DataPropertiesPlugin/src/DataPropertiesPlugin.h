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
     * Callback when is called when the selected items in the data hierarchy changes
     * @param datasetId Globally unique identifier of the dataset
     */
    void selectedItemsChanged(DataHierarchyItems selectedItems);

private:
    DataPropertiesWidget    _dataPropertiesWidget;      /** Data properties widget */
    gui::TriggerAction      _additionalEditorAction;    /** Trigger action to start the data set editor */
    Dataset<DatasetImpl>    _dataset;                   /** Smart pointer to currently selected dataset */
};

/**
 * Data properties plugin factory class
 *
 * Class which produces instances of data properties plugins
 * 
 * @author Thomas Kroes
 */
class DataPropertiesPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(mv::plugin::ViewPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "ManiVault.Plugins.View.DataPropertiesPlugin" FILE "DataPropertiesPlugin.json")
    
public:

    /** Default constructor */
    DataPropertiesPluginFactory();

    /**
     * Get plugin icon
     * @return Plugin icon
     */
    QIcon getIcon() const override;

    /** Produces an instance of a data properties plugin */
    ViewPlugin* produce() override;
};
