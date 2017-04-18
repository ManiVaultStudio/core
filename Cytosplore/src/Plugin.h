#ifndef PLUGIN_H
#define PLUGIN_H

class Plugin
{
public:
    virtual ~Plugin() {};

    virtual void init() = 0;
};

#endif // PLUGIN_H
