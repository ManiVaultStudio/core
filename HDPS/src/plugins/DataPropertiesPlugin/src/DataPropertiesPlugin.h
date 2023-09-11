// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "DataPropertiesWidget.h"

#include <ViewPlugin.h>
#include <actions/TriggerAction.h>

using namespace hdps::plugin;

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

public: // Action getters

    gui::TriggerAction* getAdditionalEditorAction() override { return &_editorAction; }

protected:

    /**
     * Callback when is called when the selected items in the data hierarchy changes
     * @param datasetId Globally unique identifier of the dataset
     */
    void selectedItemsChanged(DataHierarchyItems selectedItems);

private:
    DataPropertiesWidget    _dataPropertiesWidget;      /** Data properties widget */
    gui::TriggerAction      _editorAction;              /** Trigger action to start the view plugin editor */
    Dataset<DatasetImpl>    _dataset;                   /** Smart point to current dataset */
};

class DataPropertiesPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
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
