#pragma once

#include "Plugin.h"

#include <QObject>

namespace hdps
{
namespace plugin
{

class PluginFactory : public QObject
{
    Q_OBJECT

public:
    virtual ~PluginFactory() {};

    virtual Plugin* produce() = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::PluginFactory, "cytosplore.PluginFactory")
