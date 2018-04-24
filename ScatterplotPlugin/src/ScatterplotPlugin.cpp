#include "ScatterplotPlugin.h"

#include "ScatterplotSettings.h"
#include "PointsPlugin.h"

#include "graphics/Vector2f.h"
#include "graphics/Vector3f.h"

#include <QtCore>
#include <QtDebug>

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
            if (point.x < bounds.left()) {
                bounds.setLeft(point.x);
            }
            if (point.x > bounds.right()) {
                bounds.setRight(point.x);
            }
            if (point.y > bounds.top()) {
                bounds.setTop(point.y);
            }
            if (point.y < bounds.bottom()) {
                bounds.setBottom(point.y);
            }
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
    _scatterPlotWidget->setPointSize(10);
    _scatterPlotWidget->setSelectionColor(settings->getSelectionColor());
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
    _scatterPlotWidget->setPointSize(size);
}

void ScatterplotPlugin::sigmaChanged(const int sigma)
{
    _scatterPlotWidget->setSigma(sigma / 100.0);
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
    const IndexSet* dataSet = dynamic_cast<const IndexSet*>(_core->requestData(settings->currentData()));
    const PointsPlugin* points = dataSet->getData();
    const IndexSet* selection = dynamic_cast<const IndexSet*>(_core->requestSelection(points->getName()));
    
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

    _scatterPlotWidget->setData(&_points, getDataBounds(_points));

    updateSelection();
void ScatterplotPlugin::updateSelection()
{
    const IndexSet* dataSet = dynamic_cast<const IndexSet*>(_core->requestData(settings->currentData()));
    const PointsPlugin* points = dataSet->getData();
    const IndexSet* selection = dynamic_cast<const IndexSet*>(_core->requestSelection(points->getName()));

    std::vector<hdps::Vector3f> colors;
    colors.resize(_numPoints, settings->getBaseColor());

    if (dataSet->isFull())
    {
        for (unsigned int index : selection->indices)
        {
            colors[index] = settings->getSelectionColor();
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
                    colors[i] = settings->getSelectionColor();
                    break;
                }
            }
        }
    }

    _scatterPlotWidget->setColors(colors);
}

void ScatterplotPlugin::onSelecting(hdps::Selection selection)
{
    
}

void ScatterplotPlugin::onSelection(hdps::Selection selection)
{
    if (settings->numDataOptions() == 0)
        return;

    Selection s = _scatterPlotWidget->getSelection();

    qDebug() << "DATA SELECTION: " << s.bottomLeft().str().c_str() << s.topRight().str().c_str();

    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < _points.size(); i++)
    {
        hdps::Vector2f point = _points[i];

        if (s.contains(point))
            indices.push_back(i);
    }

    qDebug() << "INDICES SIZE: " << indices.size();
    const IndexSet* set = dynamic_cast<IndexSet*>(_core->requestData(settings->currentData()));
    IndexSet* selectionSet = dynamic_cast<IndexSet*>(_core->requestSelection(set->getDataName()));
    qDebug() << "Selection size: " << indices.size();
    selectionSet->indices.clear();
    selectionSet->indices.reserve(indices.size());

    for (unsigned int index : indices) {
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
