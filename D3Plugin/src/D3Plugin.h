#ifndef D3_PLUGIN_H
#define D3_PLUGIN_H

#include <ViewPlugin.h>
#include "D3Widget.h"

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class PointsPlugin;

class D3Plugin : public ViewPlugin, public SelectionListener
{
    Q_OBJECT
    
public:
    D3Plugin() : ViewPlugin("Scatterplot View") { }
    ~D3Plugin(void);
    
    void init();

    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved(const QString name) Q_DECL_OVERRIDE;
    void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;
    QStringList supportedDataKinds() Q_DECL_OVERRIDE;
    virtual void onSelection(const std::vector<unsigned int> selection) const;
protected slots:
    void dataSetPicked(const QString& name);

private:
    void updateData();

    D3Widget* widget;
};


// =============================================================================
// Factory
// =============================================================================

class D3PluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.D3Plugin"
                      FILE  "D3Plugin.json")
    
public:
    D3PluginFactory(void) {}
    ~D3PluginFactory(void) {}
    
    ViewPlugin* produce();
};

#endif // D3_PLUGIN_H
