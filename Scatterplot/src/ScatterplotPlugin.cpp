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

    connect(_scatterPlotWidget, &ScatterplotWidget::initialized, this, &ScatterplotPlugin::updateData);
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
    const auto& data = dataSet.getData();

    if (data.isDerivedData())
    {
        if (dataName != data.getSourceData().getName())
            return;
    }
    else
    {
        if (dataName != data.getName())
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

    if (points.isDerivedData()) {
        const PointsPlugin& sourceData = dynamic_cast<const PointsPlugin&>(points.getSourceData());

        if (points.dimNames.size() == points.getNumDimensions())
            settings->initDimOptions(points.dimNames);
        else
            settings->initDimOptions(points.getNumDimensions());

        if (sourceData.dimNames.size() == sourceData.getNumDimensions())
            settings->initScalarDimOptions(sourceData.dimNames);
        else
            settings->initScalarDimOptions(sourceData.getNumDimensions());
    }
    else
    {
        if (points.dimNames.size() == points.getNumDimensions()) {
            settings->initDimOptions(points.dimNames);
            settings->initScalarDimOptions(points.dimNames);
        }
        else {
            settings->initDimOptions(points.getNumDimensions());
            settings->initScalarDimOptions(points.getNumDimensions());
        }
    }

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
    if (!_scatterPlotWidget->isInitialized())
        return;

    if (settings->currentData().isEmpty())
        return;

    const IndexSet& dataSet = dynamic_cast<const IndexSet&>(_core->requestSet(settings->currentData()));
    const PointsPlugin& points = dataSet.getData();
    
    int nDim = points.getNumDimensions();

    int xDim = settings->getXDimension();
    int yDim = settings->getYDimension();

    qDebug() << "X: " << xDim << " Y: " << yDim;
    if (xDim < 0 || yDim < 0)
        return;

    // Determine number of points depending on if its a full dataset or a subset
    _numPoints = dataSet.isFull() ? points.getNumPoints() : dataSet.indices.size();

    calculatePositions(dataSet);

    std::vector<float> scalars(_numPoints);
    if (points.isDerivedData())
    {
        const PointsPlugin& sourceData = dynamic_cast<const PointsPlugin&>(points.getSourceData());
        calculateScalars(scalars, sourceData);
    }
    else
    {
        calculateScalars(scalars, points);
    }

    _scatterPlotWidget->setData(&_points, getDataBounds(_points));
    _scatterPlotWidget->setScalarProperty(scalars);

    updateSelection();
}

void ScatterplotPlugin::calculatePositions(const IndexSet& dataSet)
{
    const PointsPlugin& points = dataSet.getData();

    int numDims = points.getNumDimensions();
    int xDim = settings->getXDimension();
    int yDim = settings->getYDimension();

    _points.resize(_numPoints);

    if (dataSet.isFull())
    {
        for (int i = 0; i < _numPoints; i++)
        {
            _points[i].set(points.data[i * numDims + xDim], points.data[i * numDims + yDim]);
        }
    }
    else
    {
        for (int i = 0; i < _numPoints; i++)
        {
            int setIndex = dataSet.indices[i];
            _points[i].set(points.data[setIndex * numDims + xDim], points.data[setIndex * numDims + yDim]);
        }
    }
}

void ScatterplotPlugin::calculateScalars(std::vector<float>& scalars, const PointsPlugin& points)
{
    int colorIndex = settings->getColorDimension();

    if (colorIndex >= 0) {
        int numDims = points.getNumDimensions();
        float minScalar = FLT_MAX, maxScalar = FLT_MIN;

        for (int i = 0; i < _numPoints; i++) {
            float scalar = points.data[i * numDims + colorIndex];

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
}

void ScatterplotPlugin::updateSelection()
{
    const IndexSet& dataSet = dynamic_cast<const IndexSet&>(_core->requestSet(settings->currentData()));
    const RawData& data = _core->requestData(dataSet.getDataName());
    const IndexSet& selection = dynamic_cast<const IndexSet&>(data.getSelection());

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
            unsigned int index = dataSet.indices[i];
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
    const RawData& data = set.getData();

    IndexSet& selectionSet = dynamic_cast<IndexSet&>(data.getSelection());

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
