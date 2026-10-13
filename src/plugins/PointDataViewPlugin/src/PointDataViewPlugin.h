// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "PointDataViewWidget.h"

#include <ViewPlugin.h>
#include <Task.h>

class PointDataViewPlugin final : public mv::plugin::ViewPlugin
{
    Q_OBJECT

public:
    explicit PointDataViewPlugin(const mv::plugin::PluginFactory* factory);
    void init() override;

private:
    mv::Task                    _tileTask;
    PointDataViewWidget _widget;
};

class PointDataViewPluginFactory final : public mv::plugin::ViewPluginFactory
{
    Q_INTERFACES(mv::plugin::ViewPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "studio.manivault.PointDataViewPlugin" FILE "PluginInfo.json")

public:
    PointDataViewPluginFactory();
    QUrl getRepositoryUrl() const override;
    mv::plugin::ViewPlugin* produce() override;
};
