// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <actions/DatasetPickerAction.h>
#include <actions/HorizontalGroupAction.h>
#include <actions/ToggleAction.h>

#include <ViewPlugin.h>
#include <PointsViewWidget.h>

/**
 * @brief View plugin that presents point data in a lazy-fetching table.
 *
 * The plugin provides a focused consumer for the point-data access functions.
 * It can follow the data hierarchy, use an explicit Points dataset selected
 * through the picker, or accept one dropped onto the view. Keeping selection
 * policy in the plugin allows the reusable table widget to remain dataset-bound
 * and independent of global hierarchy state.
 *
 * @author Thomas Kroes
 */
class PointsViewPlugin final : public mv::plugin::ViewPlugin
{
    Q_OBJECT

public:
    /** Constructs the points view plugin. */
    explicit PointsViewPlugin(const mv::plugin::PluginFactory* factory);
    void init() override;

private:
    void updateDatasetSelection();
    void refreshSelection();
    void setPickedPoints(const mv::Dataset<Points>& points);

    mv::gui::HorizontalGroupAction _datasetSourceGroupAction;     /**< Dataset picker and hierarchy-selection controls. */
    mv::gui::ToggleAction          _useHierarchySelectionAction; /**< Whether to follow the data hierarchy selection. */
    mv::gui::DatasetPickerAction   _datasetPickerAction;         /**< Current point-dataset picker. */
    PointsViewWidget               _widget;                      /**< Embedded diagnostic table and point-data query view. */
};

/**
 * @brief Factory for creating PointsViewPlugin instances.
 *
 * The factory is the plugin-system integration point and keeps construction
 * of the diagnostic view separate from the view implementation itself.
 *
 * @author Thomas Kroes
 */
class PointsViewPluginFactory final : public mv::plugin::ViewPluginFactory
{
    Q_INTERFACES(mv::plugin::ViewPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "studio.manivault.PointsViewPlugin" FILE "PluginInfo.json")

public:
    /** Constructs the plugin factory. */
    PointsViewPluginFactory();

    /** Returns the repository URL for the plugin. */
    QUrl getRepositoryUrl() const override;

    /** Creates a points view plugin instance. */
    mv::plugin::ViewPlugin* produce() override;
};
