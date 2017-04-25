#include "CSPLR_ExampleDataPlugin.h"

#include <QtCore>
#include <QtDebug>

#include <QColor>

Q_PLUGIN_METADATA(IID "nl.tudelft.CSPLR_ExampleDataPlugin")

// =============================================================================
// Data
// =============================================================================

CSPLR_ExampleDataPlugin::~CSPLR_ExampleDataPlugin(void)
{
    
}

void CSPLR_ExampleDataPlugin::init()
{
}

void CSPLR_ExampleDataPlugin::setData(QImage image) {
    for (unsigned int y; y < image.height(); y++) {
        for (unsigned int x; x < image.width(); x++) {
            QRgb rgb = image.pixel(x, y);
            QColor color = QColor::fromRgb(rgb);
            data.push_back(color);
        }
    }
}

// =============================================================================
// Factory
// =============================================================================

DataTypePlugin* CSPLR_ExampleDataPluginFactory::produce()
{
    return new CSPLR_ExampleDataPlugin();
}
