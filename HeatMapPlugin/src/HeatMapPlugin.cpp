#include "HeatMapPlugin.h"

#include "HeatMapWidget.h"
#include "PointsPlugin.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.HeatMapPlugin")

// =============================================================================
// View
// =============================================================================

HeatMapPlugin::~HeatMapPlugin(void)
{
    
}

void HeatMapPlugin::init()
{
    widget = new HeatMapWidget();
    widget->setPage(":/heatmap/heatmap.html", "qrc:/heatmap/");
    widget->addSelectionListener(this);

    addWidget(widget);
}

void HeatMapPlugin::dataAdded(const QString name)
{

}

void HeatMapPlugin::dataChanged(const QString name)
{

}

void HeatMapPlugin::dataRemoved(const QString name)
{
    
}

void HeatMapPlugin::selectionChanged(const QString dataName)
{

}

QStringList HeatMapPlugin::supportedDataKinds()
{
    QStringList supportedKinds;
    supportedKinds << "Points";
    return supportedKinds;
}

void HeatMapPlugin::dataSetPicked(const QString& name)
{

}

void HeatMapPlugin::updateData()
{

}

void HeatMapPlugin::onSelection(const std::vector<unsigned int> selection) const
{

}

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* HeatMapPluginFactory::produce()
{
    return new HeatMapPlugin();
}
