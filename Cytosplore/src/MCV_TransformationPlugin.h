#ifndef MCV_TRANSFORMATION_PLUGIN
#define MCV_TRANSFORMATION_PLUGIN

#include <QObject>

class MCV_TransformationPlugin : public QObject
{
    Q_OBJECT
    
public:
    
    virtual ~MCV_TransformationPlugin() {};
};


class MCV_TransformationPluginFactory : public QObject
{
    Q_OBJECT
    
public:
    
    virtual ~MCV_TransformationPluginFactory() {};
    
    virtual QSharedPointer<MCV_TransformationPluginFactory> produce() = 0;
};

Q_DECLARE_INTERFACE(MCV_TransformationPluginFactory, "cytosplore.MCV_TransformationPluginFactory")

#endif // MCV_TRANSFORMATION_PLUGIN
