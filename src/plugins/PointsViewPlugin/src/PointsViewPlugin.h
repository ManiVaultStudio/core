// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "PointsViewWidget.h"

#include <ViewPlugin.h>
#include <Task.h>

class PointsViewPlugin final : public mv::plugin::ViewPlugin
{
    Q_OBJECT

public:
    explicit PointsViewPlugin(const mv::plugin::PluginFactory* factory);
    void init() override;

private:
    mv::Task                    _tileTask;
    PointsViewWidget _widget;
};

class PointsViewPluginFactory final : public mv::plugin::ViewPluginFactory
{
    Q_INTERFACES(mv::plugin::ViewPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "studio.manivault.PointsViewPlugin" FILE "PluginInfo.json")

public:
    PointsViewPluginFactory();
    QUrl getRepositoryUrl() const override;
    mv::plugin::ViewPlugin* produce() override;
};
