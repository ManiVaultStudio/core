// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "PointsViewWidget.h"

#include <ViewPlugin.h>
#include <Task.h>

/**
 * @brief View plugin that presents point data in a lazy-fetching table.
 *
 * The plugin provides a small, intentionally focused consumer for the new
 * point-data access functions. Keeping this experiment in a view plugin
 * allows the storage API to be exercised with real datasets without coupling
 * the core data hierarchy to a particular table implementation.
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
    mv::Task         _tileTask; /**< Progress task used while creating tiled storage. */
    PointsViewWidget _widget;   /**< Embedded diagnostic table and facade-access view. */
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
