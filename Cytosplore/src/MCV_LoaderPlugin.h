#ifndef MCV_LOADER_PLUGIN
#define MCV_LOADER_PLUGIN

#include <QObject>

class MCV_LoaderPlugin : public QObject
{
    Q_OBJECT

public:
    
    virtual ~MCV_LoaderPlugin() {};
};


class MCV_LoaderPluginFactory : public QObject
{
    Q_OBJECT
    
public:
    
    virtual ~MCV_LoaderPluginFactory() {};
    
    virtual QSharedPointer<MCV_LoaderPlugin> produce() = 0;
};

Q_DECLARE_INTERFACE(MCV_LoaderPluginFactory, "cytosplore.MCV_LoaderPluginFactory")

#endif // MCV_LOADER_PLUGIN
