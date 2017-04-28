#include "ScatterplotPlugin.h"

#include "ScatterplotDrawer.h"

#include <QtCore>
#include <QtDebug>

#include <QHBoxLayout>

Q_PLUGIN_METADATA(IID "nl.tudelft.ScatterplotPlugin")

// =============================================================================
// View
// =============================================================================

ScatterplotPlugin::~ScatterplotPlugin(void)
{
    
}

void ScatterplotPlugin::init()
{
    ScatterplotDrawer* drawer = new ScatterplotDrawer();

    setWidget(drawer);
}

void ScatterplotPlugin::dataAdded(const DataTypePlugin& data)
{

}

void ScatterplotPlugin::dataChanged(const DataTypePlugin& data)
{

}

void ScatterplotPlugin::dataRemoved()
{

}

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* ScatterplotPluginFactory::produce()
{
    return new ScatterplotPlugin();
}
