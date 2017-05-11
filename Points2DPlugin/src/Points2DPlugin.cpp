#include "Points2DPlugin.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.Points2DPlugin")

// =============================================================================
// View
// =============================================================================

Points2DPlugin::~Points2DPlugin(void)
{
    
}

void Points2DPlugin::init()
{
    srand(0);
    for (int i = 0; i < 1000; i++)
    {
        float x = (float)(rand() % 1000 - 500) / 500;
        float y = (float)(rand() % 1000 - 500) / 500;

        float len = sqrt(x*x + y*y);
        if (len > 0.8) {
            continue;
        }
        data.push_back(x);
        data.push_back(y);
    }
    qDebug() << data.size();
}

// =============================================================================
// Factory
// =============================================================================

DataTypePlugin* Points2DPluginFactory::produce()
{
    return new Points2DPlugin();
}
