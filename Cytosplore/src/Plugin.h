#ifndef HDPS_PLUGIN_PLUGIN_H
#define HDPS_PLUGIN_PLUGIN_H

namespace hdps {

namespace plugin {

class Plugin
{
public:
    virtual ~Plugin() {};

    virtual void init() = 0;
};

} // namespace plugin

} // namespace hdps

#endif // HDPS_PLUGIN_PLUGIN_H
