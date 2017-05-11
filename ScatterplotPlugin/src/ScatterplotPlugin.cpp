#include "ScatterplotPlugin.h"

#include "ScatterplotDrawer.h"
#include "Points2DPlugin.h"

#include <QtCore>
#include <QtDebug>

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

    _core->addData("2D Points");
}

void ScatterplotPlugin::dataAdded(const DataTypePlugin& data)
{
    if (data.getName() == "2D Points") {
        qDebug() << "Data Added";
        const Points2DPlugin& points = dynamic_cast<const Points2DPlugin&>(data);
        qDebug() << "Points: " << points.data.size();
        drawer->setData(points.data);
    }
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
