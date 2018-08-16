#include "ImageViewPlugin.h"

#include "widgets/ImageWidget.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.ImageViewPlugin")

// =============================================================================
// View
// =============================================================================

ImageViewPlugin::~ImageViewPlugin(void)
{
    
}

void ImageViewPlugin::init()
{
    widget = new ImageWidget();

    connect(&dataOptions, SIGNAL(currentIndexChanged(QString)), SLOT(dataSetPicked(QString)));
    addWidget(&dataOptions);
    addWidget(widget);
}

void ImageViewPlugin::dataAdded(const QString name)
{

}

void ImageViewPlugin::dataChanged(const QString name)
{

}

void ImageViewPlugin::dataRemoved(const QString name)
{
    
}

void ImageViewPlugin::dataSetPicked(const QString& name)
{

}

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* ImageViewPluginFactory::produce()
{
    return new ImageViewPlugin();
}
