#include "ExampleDataPlugin.h"

#include <QtCore>
#include <QtDebug>

#include <QColor>

Q_PLUGIN_METADATA(IID "nl.tudelft.ExampleDataPlugin")

// =============================================================================
// Data
// =============================================================================

ExampleDataPlugin::~ExampleDataPlugin(void)
{
    
}

void ExampleDataPlugin::init()
{
}

hdps::Set* ExampleDataPlugin::createSet() const
{
    return new PixelSet(_core, getName());
}

void ExampleDataPlugin::setData(QImage image) {
    for (unsigned int y; y < image.height(); y++) {
        for (unsigned int x; x < image.width(); x++) {
            QRgb rgb = image.pixel(x, y);
            QColor color = QColor::fromRgb(rgb);
            data.push_back(color);
        }
    }
}

hdps::Set* PixelSet::copy() const
{
    PixelSet* set = new PixelSet(_core, getDataName());
    set->setName(getName());
    set->indices = indices;
    return set;
}

// =============================================================================
// Factory
// =============================================================================

RawData* ExampleDataPluginFactory::produce()
{
    return new ExampleDataPlugin();
}
