#include "ScatterplotPlugin.h"

#include "ScatterplotSettings.h"
#include "PointsPlugin.h"

#include "graphics/Vector2f.h"
#include "graphics/Vector3f.h"

#include <QtCore>
#include <QtDebug>

#include <algorithm>
#include <limits>

Q_PLUGIN_METADATA(IID "nl.tudelft.ScatterplotPlugin")

using namespace hdps;

namespace
{
    QRectF getDataBounds(const std::vector<Vector2f>& points)
    {
        QRectF bounds;
        bounds.setLeft(FLT_MAX);
        bounds.setRight(FLT_MIN);
        bounds.setTop(FLT_MIN);
        bounds.setBottom(FLT_MAX);
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
    _scatterPlotWidget = new ScatterplotWidget();
    _scatterPlotWidget->setAlpha(0.5f);
    _scatterPlotWidget->addSelectionListener(this);

    _scatterPlotWidget->setRenderMode(ScatterplotWidget::RenderMode::SCATTERPLOT);

    settings = new ScatterplotSettings(this);

    addWidget(_scatterPlotWidget);
    addWidget(settings);
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
    const IndexSet& dataSet = dynamic_cast<const IndexSet&>(_core->requestSet(settings->currentData()));
    
    if (dataName != dataSet.getDataName()) {
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
    const IndexSet& dataSet = dynamic_cast<const IndexSet&>(_core->requestSet(settings->currentData()));
    const PointsPlugin& points = dataSet.getData();

    int nDim = points.getNumDimensions();

    settings->initDimOptions(nDim);

    updateData();
}

void ScatterplotPlugin::subsetCreated()
{
    qDebug() << "Creating subset";
    const hdps::Set& set = _core->requestSet(settings->currentData());
    const hdps::Set& selection = _core->requestSelection(set.getDataName());
    _core->createSubsetFromSelection(selection, "Subset");
}

void ScatterplotPlugin::xDimPicked(int index)
{
    updateData();
}

void ScatterplotPlugin::yDimPicked(int index)
{
    updateData();
}

void ScatterplotPlugin::cDimPicked(int index)
{
    updateData();
}

void ScatterplotPlugin::updateData()
{
    const IndexSet& dataSet = dynamic_cast<const IndexSet&>(_core->requestSet(settings->currentData()));
    const PointsPlugin& points = dataSet.getData();
    
    int nDim = points.getNumDimensions();

    int xIndex = settings->getXDimension();
    int yIndex = settings->getYDimension();
    int colorIndex = settings->getColorDimension();
    qDebug() << "X: " << xIndex << " Y: " << yIndex;
    if (xIndex < 0 || yIndex < 0)
        return;

    // Determine number of points depending on if its a full dataset or a subset
    _numPoints = dataSet.isFull() ? points.getNumPoints() : dataSet.indices.size();

    _points.resize(_numPoints);

    if (dataSet.isFull())
    {
        for (int i = 0; i < _numPoints; i++)
        {
            _points[i] = hdps::Vector2f(points.data[i * nDim + xIndex], points.data[i * nDim + yIndex]);
        }
    }
    else
    {
        for (int i = 0; i < _numPoints; i++)
        {
            int index = dataSet.indices[i];
            _points[i] = hdps::Vector2f(points.data[index * nDim + xIndex], points.data[index * nDim + yIndex]);
        }
    }

    std::vector<float> scalars(_numPoints);
    if (colorIndex >= 0) {
        float minScalar = FLT_MAX, maxScalar = FLT_MIN;
        
        for (int i = 0; i < _numPoints; i++) {
            float scalar = points.data[i * nDim + colorIndex];
            minScalar = scalar < minScalar ? scalar : minScalar;
            maxScalar = scalar > maxScalar ? scalar : maxScalar;
            scalars[i] = scalar;
        }
        // Normalize the scalars
        float scalarRange = maxScalar - minScalar;
        for (int i = 0; i < scalars.size(); i++) {
            scalars[i] = (scalars[i] - minScalar) / scalarRange;
        }
    }

    _scatterPlotWidget->setData(&_points, getDataBounds(_points));
    _scatterPlotWidget->setScalarProperty(scalars);

    updateSelection();
}

void ScatterplotPlugin::updateSelection()
{
    const IndexSet& dataSet = dynamic_cast<const IndexSet&>(_core->requestSet(settings->currentData()));
    const RawData& data = _core->requestData(dataSet.getDataName());
    const IndexSet& selection = dynamic_cast<const IndexSet&>(_core->requestSelection(data.isDerivedData() ? data.getSourceData() : dataSet.getDataName()));

    std::vector<char> highlights;
    highlights.resize(_numPoints, 0);

    if (dataSet.isFull())
    {
        for (unsigned int index : selection.indices)
        {
            highlights[index] = 1;
        }
    }
    else
    {
        for (int i = 0; i < _numPoints; i++)
        {
            int index = dataSet.indices[i];
            bool selected = false;
            for (unsigned int selectionIndex : selection.indices)
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

    const IndexSet& set = dynamic_cast<IndexSet&>(_core->requestSet(settings->currentData()));
    const RawData& data = _core->requestData(set.getDataName());

    IndexSet& selectionSet = dynamic_cast<IndexSet&>(_core->requestSelection(data.isDerivedData() ? data.getSourceData() : set.getDataName())); // TODO put this in core

    selectionSet.indices.clear();
    selectionSet.indices.reserve(indices.size());

    for (const unsigned int& index : indices) {
        selectionSet.indices.push_back(set.isFull() ? index : set.indices[index]);
    }

    _core->notifySelectionChanged(selectionSet.getDataName());
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
