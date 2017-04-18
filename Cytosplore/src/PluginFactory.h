#ifndef PLUGIN_FACTORY_H
#define PLUGIN_FACTORY_H

#include "Plugin.h"

#include <QObject>
#include <QSharedPointer>

class PluginFactory : public QObject
{
    Q_OBJECT

public:
    virtual ~PluginFactory() {};

    virtual Plugin* produce() = 0;
};

#endif // PLUGIN_FACTORY_H
