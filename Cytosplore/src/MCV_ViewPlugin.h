#ifndef MCV_VIEW_PLUGIN
#define MCV_VIEW_PLUGIN

#include "UI_DockableWidget.h"
#include "PluginFactory.h"

class UI_DockableWidget;

class MCV_ViewPlugin : public UI_DockableWidget, public Plugin
{
    Q_OBJECT
    
public:
    
    virtual ~MCV_ViewPlugin() {};
    
    virtual void dataAdded() = 0;
    virtual void dataRefreshed() = 0;
};

class MCV_ViewPluginFactory : public PluginFactory
{
    Q_OBJECT

public:

    virtual ~MCV_ViewPluginFactory() {};

    virtual MCV_ViewPlugin* produce() = 0;
};

Q_DECLARE_INTERFACE(MCV_ViewPluginFactory, "cytosplore.MCV_ViewPluginFactory")

#endif // MCV_VIEW_PLUGIN
