#include "ScatterplotPlugin.h"

#include "ScatterplotSettings.h"
#include "PointsPlugin.h"

#include "graphics/Vector2f.h"
#include "graphics/Vector3f.h"

#include <QtCore>
#include <QtDebug>

#include <algorithm>

Q_PLUGIN_METADATA(IID "nl.tudelft.ScatterplotPlugin")

using namespace hdps;

namespace
{
    QRectF getDataBounds(const std::vector<Vector2f>& points)
    {
        QRectF bounds;
        bounds.setLeft(10000);
        bounds.setRight(-10000);
        bounds.setTop(-10000);
        bounds.setBottom(10000);
        float maxDimension = 0;
        for (const Vector2f& point : points)
        {
            bounds.setLeft(std::min(point.x, (float) bounds.left()));
            bounds.setRight(std::max(point.x, (float)bounds.right()));
            bounds.setBottom(std::min(point.y, (float)bounds.bottom()));
            bounds.setTop(std::max(point.y, (float)bounds.top()));
        }
        return bounds;
    }
}

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
    _scatterPlotWidget->setPointSize(1);
    //_scatterPlotWidget->setSelectionColor(settings->getSelectionColor());
    _scatterPlotWidget->setAlpha(0.5f);
    _scatterPlotWidget->addSelectionListener(this);

    _scatterPlotWidget->setSigma(0.30f);
    _scatterPlotWidget->setRenderMode(hdps::gui::ScatterplotWidget::RenderMode::SCATTERPLOT);

    addWidget(_scatterPlotWidget);
    addWidget(settings);
}

unsigned int ScatterplotPlugin::pointSize() const
{
    return _pointSize;
}

unsigned int ScatterplotPlugin::sigma() const
{
    return _sigma;
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
    const IndexSet* dataSet = dynamic_cast<const IndexSet*>(_core->requestData(settings->currentData()));
    
    if (dataName != dataSet->getDataName()) {
        return;
    }

    updateSelection();
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
    _scatterPlotWidget->setPointSize(size / 10.0f);
}

void ScatterplotPlugin::sigmaChanged(const int sigma)
{
    _scatterPlotWidget->setSigma(sigma / 100.0f);
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
    case 0: _scatterPlotWidget->setRenderMode(hdps::gui::ScatterplotWidget::RenderMode::SCATTERPLOT); settings->showPointSettings(); break;
    case 1: _scatterPlotWidget->setRenderMode(hdps::gui::ScatterplotWidget::RenderMode::DENSITY); settings->showDensitySettings(); break;
    case 2: _scatterPlotWidget->setRenderMode(hdps::gui::ScatterplotWidget::RenderMode::LANDSCAPE); settings->showDensitySettings(); break;
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
    qDebug() << "UPdate data";
    const IndexSet* dataSet = dynamic_cast<const IndexSet*>(_core->requestData(settings->currentData()));
    const PointsPlugin* points = dataSet->getData();
    
    int nDim = points->getNumDimensions();

    int xIndex = settings->getXDimension();
    int yIndex = settings->getYDimension();
    qDebug() << "X: " << xIndex << " Y: " << yIndex;
    if (xIndex < 0 || yIndex < 0)
        return;

    // Determine number of points depending on if its a full dataset or a subset
    _numPoints = dataSet->isFull() ? points->getNumPoints() : dataSet->indices.size();

    _points.resize(_numPoints);

    if (dataSet->isFull())
    {
        for (int i = 0; i < _numPoints; i++)
        {
            _points[i] = hdps::Vector2f(points->data[i * nDim + xIndex], points->data[i * nDim + yIndex]);
        }
    }
    else
    {
        for (int i = 0; i < _numPoints; i++)
        {
            int index = dataSet->indices[i];
            _points[i] = hdps::Vector2f(points->data[index * nDim + xIndex], points->data[index * nDim + yIndex]);
        }
    }
    std::vector<float> scalars(_numPoints);
    for (int i = 0; i < _numPoints; i++) {
        scalars[i] = (float)i / _numPoints;
    }

    _scatterPlotWidget->setData(&_points, getDataBounds(_points));
    _scatterPlotWidget->setScalarProperty(scalars);

    std::vector<hdps::Vector3f> colors;
    colors.resize(_numPoints, settings->getBaseColor());
    _scatterPlotWidget->setColors(colors);

    updateSelection();
}

void ScatterplotPlugin::updateSelection()
{
    const IndexSet* dataSet = dynamic_cast<const IndexSet*>(_core->requestData(settings->currentData()));
    const DataTypePlugin* data = _core->requestPlugin(dataSet->getDataName());
    const IndexSet* selection = nullptr;
    qDebug() << "UPdate selection";
    if (data->isDerivedData()) {
        selection = dynamic_cast<const IndexSet*>(_core->requestSelection(data->getSourceData()));
    }
    else {
        selection = dynamic_cast<const IndexSet*>(_core->requestSelection(dataSet->getDataName()));
    }
    qDebug() << "Past UPdate selection";
    std::vector<char> highlights;
    highlights.resize(_numPoints, 0);

    if (dataSet->isFull())
    {
        for (unsigned int index : selection->indices)
        {
            highlights[index] = 1;
        }
    }
    else
    {
        for (int i = 0; i < _numPoints; i++)
        {
            int index = dataSet->indices[i];
            bool selected = false;
            for (unsigned int selectionIndex : selection->indices)
            {
                if (index == selectionIndex) {
                    highlights[i] = 1;
                    break;
                }
            }
        }
    }

    _scatterPlotWidget->setHighlights(highlights);
}

void ScatterplotPlugin::makeSelection(hdps::Selection selection)
{
    if (settings->numDataOptions() == 0)
        return;

    Selection s = _scatterPlotWidget->getSelection();

    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < _points.size(); i++)
    {
        const hdps::Vector2f& point = _points[i];

        if (s.contains(point))
            indices.push_back(i);
    }

    const IndexSet* set = dynamic_cast<IndexSet*>(_core->requestData(settings->currentData()));
    IndexSet* selectionSet = dynamic_cast<IndexSet*>(_core->requestSelection(set->getDataName()));

    selectionSet->indices.clear();
    selectionSet->indices.reserve(indices.size());

    for (const unsigned int& index : indices) {
        selectionSet->indices.push_back(set->isFull() ? index : set->indices[index]);
    }

    _core->notifySelectionChanged(selectionSet->getDataName());
}

void ScatterplotPlugin::onSelecting(hdps::Selection selection)
{
    makeSelection(selection);
}

void ScatterplotPlugin::onSelection(hdps::Selection selection)
{
    makeSelection(selection);
}

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* ScatterplotPluginFactory::produce()
{
    return new ScatterplotPlugin();
}
