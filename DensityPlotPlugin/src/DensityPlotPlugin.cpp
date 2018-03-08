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

using RenderMode = hdps::gui::DensityPlotWidget::RenderMode;

DensityPlotPlugin::DensityPlotPlugin()
    :
    ViewPlugin("Density Plot"),
    _widget(RenderMode::DENSITY)
{

}

DensityPlotPlugin::~DensityPlotPlugin()
{
    
}

void DensityPlotPlugin::init()
{
    _settings = new DensityPlotSettings(this);

    _widget.setSigma(0.30f);

    addWidget(&_widget);
    addWidget(_settings);
}

void DensityPlotPlugin::dataAdded(const QString name)
{
    _settings->addDataOption(name);
}

void DensityPlotPlugin::dataChanged(const QString name)
{
    if (name != _settings->currentData()) {
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
    const IndexSet* dataSet = dynamic_cast<const IndexSet*>(_core->requestData(_settings->currentData()));
    const PointsPlugin* points = dynamic_cast<const PointsPlugin*>(_core->requestPlugin(dataSet->getDataName()));

    int nDim = points->getNumDimensions();

    _settings->initDimOptions(nDim);

    updateData();
}

void DensityPlotPlugin::renderModePicked(const int index)
{
    switch (index)
    {
    case 0: _widget->setRenderMode(hdps::gui::DensityPlotWidget::DENSITY); break;
    case 1: _widget->setRenderMode(hdps::gui::DensityPlotWidget::GRADIENT); break;
    case 2: _widget->setRenderMode(hdps::gui::DensityPlotWidget::PARTITIONS); break;
    case 3: _widget->setRenderMode(hdps::gui::DensityPlotWidget::LANDSCAPE); break;
    }
}

void DensityPlotPlugin::sigmaChanged(const int size)
{
    _widget.setSigma(size / 100.f);
    _widget.update();
}

void DensityPlotPlugin::subsetCreated()
{
    qDebug() << "Creating subset";
    const hdps::Set* set = _core->requestData(_settings->currentData());
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
    const IndexSet* dataSet = dynamic_cast<const IndexSet*>(_core->requestData(_settings->currentData()));
    const PointsPlugin* points = dataSet->getData();
    const IndexSet* selection = dynamic_cast<const IndexSet*>(_core->requestSelection(points->getName()));
    
    std::vector<hdps::Vector2f>* positions = new std::vector<hdps::Vector2f>();

    int nDim = points->getNumDimensions();

    int xIndex = _settings->getXDimension();
    int yIndex = _settings->getYDimension();
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
    _widget.setData(positions);
}

void DensityPlotPlugin::onSelection(const std::vector<unsigned int> selection) const
{
    if (_settings->numDataOptions() == 0)
        return;

    const IndexSet* set = dynamic_cast<IndexSet*>(_core->requestData(_settings->currentData()));
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
    int xIndex = _settings->getXDimension();
    int yIndex = _settings->getYDimension();

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
