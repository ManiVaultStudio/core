#include "D3Plugin.h"

#include "D3Widget.h"
#include "PointsPlugin.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.D3Plugin")

// =============================================================================
// View
// =============================================================================

D3Plugin::~D3Plugin(void)
{
    
}

void D3Plugin::init()
{
    widget = new D3Widget();
    widget->addSelectionListener(this);

    addWidget(widget);
}

void D3Plugin::dataAdded(const QString name)
{

}

void D3Plugin::dataChanged(const QString name)
{

}

void D3Plugin::dataRemoved(const QString name)
{
    
}

void D3Plugin::selectionChanged(const QString dataName)
{

}

QStringList D3Plugin::supportedDataKinds()
{
    QStringList supportedKinds;
    supportedKinds << "Points";
    return supportedKinds;
}

void D3Plugin::dataSetPicked(const QString& name)
{

}

void D3Plugin::updateData()
{

}

void D3Plugin::onSelection(const std::vector<unsigned int> selection) const
{

}

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* D3PluginFactory::produce()
{
    return new D3Plugin();
}
