#include "ScatterplotPlugin.h"

#include "ScatterplotDrawer.h"

#include <QtCore>
#include <QtDebug>

#include <vector>

Q_PLUGIN_METADATA(IID "nl.tudelft.ScatterplotPlugin")

// =============================================================================
// View
// =============================================================================

ScatterplotPlugin::~ScatterplotPlugin(void)
{
    
}

void ScatterplotPlugin::init()
{
    drawer = new ScatterplotDrawer();

    setWidget(drawer);

    dataRemoved();
}

void ScatterplotPlugin::dataAdded(const DataTypePlugin& data)
{

}

void ScatterplotPlugin::dataChanged(const DataTypePlugin& data)
{

}

void ScatterplotPlugin::dataRemoved()
{
    std::vector<float> positions;

    srand(0);
    for (int i = 0; i < 1000; i++)
    {
        float x = (float)(rand() % 1000 - 500) / 500;
        float y = (float)(rand() % 1000 - 500) / 500;

        float len = sqrt(x*x + y*y);
        if (len > 0.8) {
            continue;
        }
        positions.push_back(x);
        positions.push_back(y);
    }

    drawer->setData(positions);
}

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* ScatterplotPluginFactory::produce()
{
    return new ScatterplotPlugin();
}
