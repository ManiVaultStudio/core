#ifndef MCV_DATA_TYPE_PLUGIN
#define MCV_DATA_TYPE_PLUGIN

#include <QObject>

class MCV_DataTypePlugin : public QObject
{
    Q_OBJECT

public:
    
    virtual ~MCV_DataTypePlugin() {};
};


class MCV_DataTypePluginFactory : public QObject
{
    Q_OBJECT
    
public:
    
    virtual ~MCV_DataTypePluginFactory() {};
    
    virtual QSharedPointer<MCV_DataTypePlugin> produce() = 0;
};

Q_DECLARE_INTERFACE(MCV_DataTypePluginFactory, "cytosplore.MCV_DataTypePluginFactory")

#endif // MCV_DATA_TYPE_PLUGIN
