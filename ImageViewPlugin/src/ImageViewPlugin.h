#ifndef IMAGE_VIEW_PLUGIN_H
#define IMAGE_VIEW_PLUGIN_H

#include <ViewPlugin.h>

#include <QComboBox>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class ImageWidget;

class ImageViewPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:
    ImageViewPlugin() : ViewPlugin("Image View") { }
    ~ImageViewPlugin(void);
    
    void init();
    
    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved(const QString name) Q_DECL_OVERRIDE;
protected slots:
    void dataSetPicked(const QString& name);
private:
    ImageWidget* widget;

    QComboBox dataOptions;
};


// =============================================================================
// Factory
// =============================================================================

class ImageViewPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.ImageViewPlugin"
                      FILE  "ImageViewPlugin.json")
    
public:
    ImageViewPluginFactory(void) {}
    ~ImageViewPluginFactory(void) {}
    
    ViewPlugin* produce();
};

#endif // IMAGE_VIEW_PLUGIN_H
