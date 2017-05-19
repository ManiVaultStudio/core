#include "ScatterplotPlugin.h"

#include "widgets/ScatterplotWidget.h"
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
    widget = new ScatterplotWidget();
    widget->setPointSize(5);
    widget->setAlpha(0.01f);
    setWidget(widget);
}

void ScatterplotPlugin::dataAdded(const DataTypePlugin& data)
{
    if (data.getName() == "2D Points") {
        qDebug() << "Data Added for scatterplot";
        const Points2DPlugin& points = dynamic_cast<const Points2DPlugin&>(data);
        qDebug() << "Points: " << points.data.size();
        widget->setData(points.data);
    }
}

void ScatterplotPlugin::dataChanged(const DataTypePlugin& data)
{
    if (data.getName() == "2D Points") {
        qDebug() << "Data Changed for scatterplot";
        const Points2DPlugin& points = dynamic_cast<const Points2DPlugin&>(data);
        qDebug() << "Points: " << points.data.size();
        widget->setData(points.data);
    }
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
