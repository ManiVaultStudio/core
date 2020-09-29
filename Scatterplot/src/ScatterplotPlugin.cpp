#include "ScatterplotPlugin.h"

#include "ScatterplotSettings.h"
#include "PointData.h"
#include "ClusterData.h"
#include "ColorData.h"

#include "graphics/Vector2f.h"
#include "graphics/Vector3f.h"

#include <QtCore>
#include <QtDebug>

#include <algorithm>
#include <limits>

Q_PLUGIN_METADATA(IID "nl.tudelft.ScatterplotPlugin")

using namespace hdps;

// =============================================================================
// View
// =============================================================================

ScatterplotPlugin::~ScatterplotPlugin(void)
{
    
}

void ScatterplotPlugin::init()
{
    _dataSlot = new DataSlot(supportedDataTypes());
    supportedColorTypes.append(PointType);
    supportedColorTypes.append(ClusterType);
    supportedColorTypes.append(ColorType);

    _scatterPlotWidget = new ScatterplotWidget();
    _scatterPlotWidget->setAlpha(0.5f);
    _scatterPlotWidget->addSelectionListener(this);

    _scatterPlotWidget->setRenderMode(ScatterplotWidget::RenderMode::SCATTERPLOT);
    _dataSlot->addWidget(_scatterPlotWidget);

    settings = new ScatterplotSettings(this);

    QHBoxLayout* layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    setMainLayout(layout);
    addWidget(_dataSlot);
    addWidget(settings);

    connect(_dataSlot, &DataSlot::onDataInput, this, &ScatterplotPlugin::onDataInput);
    connect(_scatterPlotWidget, &ScatterplotWidget::initialized, this, &ScatterplotPlugin::updateData);
}

void ScatterplotPlugin::dataAdded(const QString name)
{
    
}

void ScatterplotPlugin::dataChanged(const QString name)
{
    if (name != _currentDataSet) {
        return;
    }
    updateData();
}

void ScatterplotPlugin::dataRemoved(const QString name)
{
    
}

void ScatterplotPlugin::selectionChanged(const QString dataName)
{
    if (_currentDataSet.isEmpty()) return;

    const Points& points = _core->requestData<Points>(_currentDataSet);

    if (DataSet::getSourceData(points).getDataName() == dataName)
        updateSelection();
}

DataTypes ScatterplotPlugin::supportedDataTypes() const
{
    DataTypes supportedTypes;
    supportedTypes.append(PointType);
    return supportedTypes;
}

void ScatterplotPlugin::subsetCreated()
{
    const Points& points = _core->requestData<Points>(_currentDataSet);

    points.createSubset();
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
    const Points& points = DataSet::getSourceData(_core->requestData<Points>(_currentDataSet));

    std::vector<float> scalars;
    calculateScalars(scalars, points, index);

    _scatterPlotWidget->setScalars(scalars);
    _scatterPlotWidget->setScalarEffect(PointEffect::Color);
    updateData();
}

void ScatterplotPlugin::onDataInput(QString dataSetName)
{
    _currentDataSet = dataSetName;

    setWindowTitle(_currentDataSet);

    const Points& points = _core->requestData<Points>(_currentDataSet);

    if (points.getDimensionNames().size() == points.getNumDimensions())
    {
        settings->initDimOptions(points.getDimensionNames());
        settings->initScalarDimOptions(DataSet::getSourceData(points).getDimensionNames());
    }
    else
    {
        settings->initDimOptions(points.getNumDimensions());
        settings->initScalarDimOptions(DataSet::getSourceData(points).getNumDimensions());
    }

    updateData();
}

void ScatterplotPlugin::onColorDataInput(QString dataSetName)
{
    DataSet& dataSet = _core->requestData(dataSetName);

    DataType dataType = dataSet.getDataType();

    if (dataType == PointType)
    {
        Points& points = static_cast<Points&>(dataSet);

        std::vector<float> scalars;
        if (points.getNumPoints() != _numPoints)
        {
            qWarning("Number of points used for coloring does not match number of points in data, aborting attempt to color plot");
            return;
        }

        points.visitFromBeginToEnd([&scalars](auto begin, auto end)
            {
                scalars.insert(scalars.begin(), begin, end);
            });

        _scatterPlotWidget->setScalars(scalars);
        _scatterPlotWidget->setScalarEffect(PointEffect::Color);
        updateData();
    }
    if (dataType == ClusterType)
    {
        Clusters& clusters = static_cast<Clusters&>(dataSet);
        
        std::vector<Vector3f> colors(_points.size());
        for (const Cluster& cluster : clusters.getClusters())
        {
            for (const int& index : cluster.indices)
            {
                if (index < 0 || index > colors.size())
                {
                    qWarning("Cluster index is out of range of data, aborting attempt to color plot");
                    return;
                }
                colors[index] = Vector3f(cluster._color.redF(), cluster._color.greenF(), cluster._color.blueF());
            }
        }

        _scatterPlotWidget->setColors(colors);

        updateData();
    }
}

void ScatterplotPlugin::updateData()
{
    // Check if the scatter plot is initialized, if not, don't do anything
    if (!_scatterPlotWidget->isInitialized())
        return;

    // If no dataset has been selected, don't do anything
    if (_currentDataSet.isEmpty())
        return;

    // Get the dataset belonging to the currently displayed dataset
    const Points& points = _core->requestData<Points>(_currentDataSet);

    // Get the selected dimensions to use as X and Y dimension in the plot
    int xDim = settings->getXDimension();
    int yDim = settings->getYDimension();

    // If one of the dimensions was not set, do not draw anything
    if (xDim < 0 || yDim < 0)
        return;

    // Determine number of points depending on if its a full dataset or a subset
    _numPoints = points.getNumPoints();

    // Extract 2-dimensional points from the data set based on the selected dimensions
    calculatePositions(points);

    // Pass the 2D points to the scatter plot widget
    _scatterPlotWidget->setData(&_points);

    updateSelection();
}

void ScatterplotPlugin::calculatePositions(const Points& points)
{
    points.extractDataForDimensions(_points, settings->getXDimension(), settings->getYDimension());
}

void ScatterplotPlugin::calculateScalars(std::vector<float>& scalars, const Points& points, int colorIndex)
{
    if (colorIndex >= 0) {
        points.extractDataForDimension(scalars, colorIndex);
    }
}

void ScatterplotPlugin::updateSelection()
{
    const Points& points = _core->requestData<Points>(_currentDataSet);
    const Points& selection = static_cast<Points&>(points.getSelection());

    std::vector<char> highlights;
    highlights.resize(_numPoints, 0);

    if (points.isDerivedData())
    {
        // If the dataset is derived from another, get the source data set
        const Points& sourceSet = DataSet::getSourceData(points);

        if (sourceSet.isFull())
        {
            // If the dataset is the full dataset no translation is necessary for selection
            if (points.isFull())
            {
                for (unsigned int index : selection.indices)
                {
                    highlights[index] = 1;
                }
            }
        }
        else
        {
            // Use the indices from the source dataset to translate global selection to local selection
            for (int i = 0; i < _numPoints; i++)
            {
                unsigned int index = sourceSet.indices[i];
                for (unsigned int selectionIndex : selection.indices)
                {
                    if (index == selectionIndex) {
                        highlights[i] = 1;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        // If the dataset is the full dataset no translation is necessary for selection
        if (points.isFull())
        {
            for (unsigned int index : selection.indices)
            {
                highlights[index] = 1;
            }
        }
        else
        {
            // Use the indices from the subset to translate global selection to local selection
            for (int i = 0; i < _numPoints; i++)
            {
                unsigned int index = points.indices[i];
                for (unsigned int selectionIndex : selection.indices)
                {
                    if (index == selectionIndex) {
                        highlights[i] = 1;
                        break;
                    }
                }
            }
        }
    }

    _scatterPlotWidget->setHighlights(highlights);
}

// Returns the selection set associated with this dataset
const Points* ScatterplotPlugin::makeSelection(hdps::Selection selection)
{
    // If theres no dataset being displayed, don't try to make any selection
    if (_currentDataSet.isEmpty())
        return nullptr;

    // Get the selection box from the widget
    Selection s = _scatterPlotWidget->getSelection();

    // Check which local indices of points lie in the selection box
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < _points.size(); i++)
    {
        const hdps::Vector2f& point = _points[i];

        if (s.contains(point))
            indices.push_back(i);
    }

    // Get the selection set associated with this dataset
    const Points& points = _core->requestData<Points>(_currentDataSet);
    Points& selectionSet = dynamic_cast<Points&>(points.getSelection());

    // Clear the selection and reserve as many indices as there are points in the selection box
    selectionSet.indices.clear();
    selectionSet.indices.reserve(indices.size());

    // If the data is not derived then translate local indices to global indices and save them in the selection set
    if (!points.isDerivedData())
    {
        for (const unsigned int& index : indices) {
            selectionSet.indices.push_back(points.isFull() ? index : points.indices[index]);
        }
    }
    else
    {
        const Points& sourceSet = DataSet::getSourceData(points);
        for (const unsigned int& index : indices) {
            selectionSet.indices.push_back(sourceSet.isFull() ? index : sourceSet.indices[index]);
        }
    }
    return &selectionSet;
}

void ScatterplotPlugin::onSelecting(hdps::Selection selection)
{
    const Points* const selectionSet = makeSelection(selection);

    if (selectionSet != nullptr)
    {
        // Only notify other plugins of the selection changes if enabled in the settings
        // This prevents plugins that take a long time to process selections to deteriorate performance
        if ((settings != nullptr) && settings->IsNotifyingOnSelecting())
            _core->notifySelectionChanged(selectionSet->getDataName());
        else
            updateSelection();
    }
}

void ScatterplotPlugin::onSelection(hdps::Selection selection)
{
    const Points* const selectionSet = makeSelection(selection);

    if (selectionSet != nullptr)
    {
        _core->notifySelectionChanged(selectionSet->getDataName());
    }
}

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* ScatterplotPluginFactory::produce()
{
    return new ScatterplotPlugin();
}
