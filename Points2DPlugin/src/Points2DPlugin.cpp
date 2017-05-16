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
    for (int i = 0; i < 500000; i++)
    {
        float x = (float)(rand() % 30000 - 15000) / 15000;
        float y = (float)(rand() % 30000 - 15000) / 15000;

        float len = sqrt(x*x + y*y);
        float roulette = (float)(rand() % 30000 - 15000) / 15000;
        if (roulette < len * 1.4) {
            continue;
        }
        data.push_back(x);
        data.push_back(y);
    }
    qDebug() << "Generated: " << data.size() << " points.";
}

// =============================================================================
// Factory
// =============================================================================

DataTypePlugin* Points2DPluginFactory::produce()
{
    return new Points2DPlugin();
}
