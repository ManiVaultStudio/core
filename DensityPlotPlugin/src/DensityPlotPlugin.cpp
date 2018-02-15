#include "DensityPlotPlugin.h"

#include "DensityPlotSettings.h"
#include "PointsPlugin.h"

#include "graphics/Vector2f.h"
#include "graphics/Vector3f.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.DensityPlotPlugin")

// =============================================================================
// View
// =============================================================================

DensityPlotPlugin::~DensityPlotPlugin(void)
{
    
}

void DensityPlotPlugin::init()
{
    settings = new DensityPlotSettings(this);

    widget = new hdps::gui::DensityPlotWidget(hdps::gui::DensityPlotWidget::DENSITY);

    addWidget(widget);
    addWidget(settings);
}

void DensityPlotPlugin::dataAdded(const QString name)
{
    settings->addDataOption(name);
}

void DensityPlotPlugin::dataChanged(const QString name)
{
    if (name != settings->currentData()) {
        return;
    }
    updateData();
}

void DensityPlotPlugin::dataRemoved(const QString name)
{
    
}

void DensityPlotPlugin::selectionChanged(const QString dataName)
{
    qDebug() << getName() << "Selection updated";
    updateData();
}

QStringList DensityPlotPlugin::supportedDataKinds()
{
    QStringList supportedKinds;
    supportedKinds << "Points";
    return supportedKinds;
}

void DensityPlotPlugin::dataSetPicked(const QString& name)
{
    const IndexSet* dataSet = dynamic_cast<const IndexSet*>(_core->requestData(settings->currentData()));
    const PointsPlugin* points = dynamic_cast<const PointsPlugin*>(_core->requestPlugin(dataSet->getDataName()));

    int nDim = points->getNumDimensions();

    settings->initDimOptions(nDim);

    updateData();
}

void DensityPlotPlugin::subsetCreated()
{
    qDebug() << "Creating subset";
    const hdps::Set* set = _core->requestData(settings->currentData());
    const hdps::Set* selection = _core->requestSelection(set->getDataName());
    _core->createSubsetFromSelection(selection, "Subset");
    qDebug() << "Subset created.";
}

void DensityPlotPlugin::xDimPicked(int index)
{
    updateData();
}

void DensityPlotPlugin::yDimPicked(int index)
{
    updateData();
}

void DensityPlotPlugin::updateData()
{
    qDebug() << "UPDATING";
    const IndexSet* dataSet = dynamic_cast<const IndexSet*>(_core->requestData(settings->currentData()));
    const PointsPlugin* points = dataSet->getData();
    const IndexSet* selection = dynamic_cast<const IndexSet*>(_core->requestSelection(points->getName()));
    
    std::vector<hdps::Vector2f>* positions = new std::vector<hdps::Vector2f>();

    int nDim = points->getNumDimensions();

    int xIndex = settings->getXDimension();
    int yIndex = settings->getYDimension();
    qDebug() << "X: " << xIndex << " Y: " << yIndex;
    if (xIndex < 0 || yIndex < 0)
        return;

    // Calculate data bounds
    float maxLength = getMaxLength(&points->data, nDim);

    // Determine number of points depending on if its a full dataset or a subset
    unsigned int numPoints = dataSet->isFull() ? points->getNumPoints() : dataSet->indices.size();

    positions->resize(numPoints);

    if (dataSet->isFull())
    {
        for (int i = 0; i < numPoints; i++)
        {
            (*positions)[i] = hdps::Vector2f(points->data[i * nDim + xIndex], points->data[i * nDim + yIndex]) / maxLength;
        }
    }
    else
    {
        for (int i = 0; i < numPoints; i++)
        {
            int index = dataSet->indices[i];
            (*positions)[i] = hdps::Vector2f(points->data[index * nDim + xIndex], points->data[index * nDim + yIndex]) / maxLength;
        }
    }

    qDebug() << "Setting positions";
    widget->setData(positions);
    qDebug() << "Setting colors";
    //widget->setColors(colors);
    qDebug() << "DONE UPDATING";
}

void DensityPlotPlugin::onSelection(const std::vector<unsigned int> selection) const
{
    if (settings->numDataOptions() == 0)
        return;

    const IndexSet* set = dynamic_cast<IndexSet*>(_core->requestData(settings->currentData()));
    IndexSet* selectionSet = dynamic_cast<IndexSet*>(_core->requestSelection(set->getDataName()));
    qDebug() << "Selection size: " << selection.size();
    selectionSet->indices.clear();

    for (unsigned int index : selection) {
        selectionSet->indices.push_back(set->isFull() ? index : set->indices[index]);
    }
    
    qDebug() << "Selection on: " << selectionSet->getDataName();
    _core->notifySelectionChanged(selectionSet->getDataName());
}

float DensityPlotPlugin::getMaxLength(const std::vector<float>* data, const int nDim) const
{
    int xIndex = settings->getXDimension();
    int yIndex = settings->getYDimension();

    float maxLength = 0;
    for (int i = 0; i < data->size() / nDim; i++) {
        float x = data->at(i * nDim + xIndex);
        float y = data->at(i * nDim + yIndex);
        float length = x*x + y*y;

        if (length > maxLength)
            maxLength = length;
    }
    return sqrt(maxLength);
}

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* DensityPlotPluginFactory::produce()
{
    return new DensityPlotPlugin();
}
