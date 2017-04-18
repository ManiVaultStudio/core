#ifndef MCV_ANALYSIS_PLUGIN
#define MCV_ANALYSIS_PLUGIN

#include "PluginFactory.h"

class MCV_AnalysisPlugin : public Plugin
{
public:

    virtual ~MCV_AnalysisPlugin() {};
};


class MCV_AnalysisPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    
    virtual ~MCV_AnalysisPluginFactory() {};
    
    virtual MCV_AnalysisPlugin* produce() = 0;
};

Q_DECLARE_INTERFACE(MCV_AnalysisPluginFactory, "cytosplore.MCV_AnalysisPluginFactory")

#endif // MCV_ANALYSIS_PLUGIN
