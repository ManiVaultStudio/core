#include "ScatterplotPlugin.h"

#include "ScatterplotDrawer.h"

#include <QtCore>
#include <QtDebug>

#include <vector>

#define sign(x) (x < 0 ? -1 : 1)

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

        positions.push_back(sign(x) * (x*x));
        positions.push_back(sign(y) * (y*y));
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
