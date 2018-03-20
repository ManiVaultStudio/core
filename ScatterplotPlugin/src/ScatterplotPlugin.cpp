#include "ScatterplotPlugin.h"

#include "ScatterplotSettings.h"
#include "PointsPlugin.h"

#include "graphics/Vector2f.h"
#include "graphics/Vector3f.h"

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
    settings = new ScatterplotSettings(this);

    _scatterPlotWidget = new hdps::gui::ScatterplotWidget();
    _scatterPlotWidget->setPointSize(10);
    _scatterPlotWidget->setSelectionColor(settings->getSelectionColor());
    _scatterPlotWidget->setAlpha(0.5f);
    _scatterPlotWidget->addSelectionListener(this);

    addWidget(widget);
    addWidget(settings);
}

unsigned int ScatterplotPlugin::pointSize() const
{
    return _pointSize;
}

void ScatterplotPlugin::dataAdded(const QString name)
{
    settings->addDataOption(name);
}

void ScatterplotPlugin::dataChanged(const QString name)
{
    if (name != settings->currentData()) {
        return;
    }
    updateData();
}

void ScatterplotPlugin::dataRemoved(const QString name)
{
    
}

void ScatterplotPlugin::selectionChanged(const QString dataName)
{
    qDebug() << getName() << "Selection updated";
    updateData();
}

QStringList ScatterplotPlugin::supportedDataKinds()
{
    QStringList supportedKinds;
    supportedKinds << "Points";
    return supportedKinds;
}

void ScatterplotPlugin::dataSetPicked(const QString& name)
{
    const IndexSet* dataSet = dynamic_cast<const IndexSet*>(_core->requestData(settings->currentData()));
    const PointsPlugin* points = dynamic_cast<const PointsPlugin*>(_core->requestPlugin(dataSet->getDataName()));

    int nDim = points->getNumDimensions();

    settings->initDimOptions(nDim);

    updateData();
}

void ScatterplotPlugin::pointSizeChanged(const int size)
{
    _pointSize = size;
    widget->setPointSize(size);
}

void ScatterplotPlugin::subsetCreated()
{
    qDebug() << "Creating subset";
    const hdps::Set* set = _core->requestData(settings->currentData());
    const hdps::Set* selection = _core->requestSelection(set->getDataName());
    _core->createSubsetFromSelection(selection, "Subset");
}

void ScatterplotPlugin::renderModePicked(const int index)
{
    switch (index)
    {
    case 0: _scatterPlotWidget->setRenderMode(hdps::gui::ScatterplotWidget::RenderMode::SCATTERPLOT); break;
    case 1: _scatterPlotWidget->setRenderMode(hdps::gui::ScatterplotWidget::RenderMode::DENSITY); break;
    case 2: _scatterPlotWidget->setRenderMode(hdps::gui::ScatterplotWidget::RenderMode::LANDSCAPE); break;
    }
    qDebug() << "Render Mode Picked";
}

void ScatterplotPlugin::xDimPicked(int index)
{
    updateData();
}

void ScatterplotPlugin::yDimPicked(int index)
{
    updateData();
}


void ScatterplotPlugin::updateData()
{
    qDebug() << "UPDATING SCATTER PLUGIN";
    const IndexSet* dataSet = dynamic_cast<const IndexSet*>(_core->requestData(settings->currentData()));
    const PointsPlugin* points = dataSet->getData();
    const IndexSet* selection = dynamic_cast<const IndexSet*>(_core->requestSelection(points->getName()));
    
    std::vector<hdps::Vector2f>* positions = new std::vector<hdps::Vector2f>();
    std::vector<hdps::Vector3f> colors;

    int nDim = points->getNumDimensions();

    int xIndex = settings->getXDimension();
    int yIndex = settings->getYDimension();
    qDebug() << "X: " << xIndex << " Y: " << yIndex;
    if (xIndex < 0 || yIndex < 0)
        return;

    // Determine number of points depending on if its a full dataset or a subset
    unsigned int numPoints = dataSet->isFull() ? points->getNumPoints() : dataSet->indices.size();

    positions->resize(numPoints);
    colors.resize(numPoints, settings->getBaseColor());

    if (dataSet->isFull())
    {
        for (int i = 0; i < numPoints; i++)
        {
            (*positions)[i] = hdps::Vector2f(points->data[i * nDim + xIndex], points->data[i * nDim + yIndex]);
        }

        for (unsigned int index : selection->indices)
        {
            colors[index] = settings->getSelectionColor();
        }
    }
    else
    {
        for (int i = 0; i < numPoints; i++)
        {
            int index = dataSet->indices[i];
            (*positions)[i] = hdps::Vector2f(points->data[index * nDim + xIndex], points->data[index * nDim + yIndex]);

            bool selected = false;
            for (unsigned int selectionIndex : selection->indices)
            {
                if (index == selectionIndex) {
                    colors[i] = settings->getSelectionColor();
                    break;
                }
            }
        }
    }

    qDebug() << "Setting positions";
    widget->setData(positions);
    qDebug() << "Setting colors";
    widget->setColors(colors);
    qDebug() << "DONE UPDATING SCATTER";
}

void ScatterplotPlugin::onSelection(const std::vector<unsigned int> selection) const
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

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* ScatterplotPluginFactory::produce()
{
    return new ScatterplotPlugin();
}
