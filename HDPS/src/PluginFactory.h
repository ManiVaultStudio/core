#ifndef HDPS_PLUGINFACTORY_H
#define HDPS_PLUGINFACTORY_H

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
    ~PluginFactory() override {};

    virtual Plugin* produce() = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::PluginFactory, "cytosplore.PluginFactory")

#endif // HDPS_PLUGINFACTORY_H
