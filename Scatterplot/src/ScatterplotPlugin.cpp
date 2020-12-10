#include "ScatterplotPlugin.h"

#include "SettingsWidget.h"
#include "PointData.h"
#include "ClusterData.h"
#include "ColorData.h"

#include "graphics/Vector2f.h"
#include "graphics/Vector3f.h"

#include <QtCore>
#include <QApplication>
#include <QtDebug>
#include <QSplitter>

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

    _scatterPlotWidget = new ScatterplotWidget(*_selectionTool);
    _scatterPlotWidget->setAlpha(0.5f);
    _scatterPlotWidget->addSelectionListener(this);

    _scatterPlotWidget->setRenderMode(ScatterplotWidget::RenderMode::SCATTERPLOT);
    _dataSlot->addWidget(_scatterPlotWidget);

    _scatterPlotSettings = new ScatterplotSettings(this);

    auto splitter = new QSplitter();

    splitter->addWidget(_dataSlot);
    splitter->addWidget(_scatterPlotSettings);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);
    splitter->setCollapsible(1, true);

    addWidget(splitter);

    _scatterPlotWidget->installEventFilter(_selectionTool);

    connect(_dataSlot, &DataSlot::onDataInput, this, &ScatterplotPlugin::onDataInput);
    connect(_scatterPlotWidget, &ScatterplotWidget::initialized, this, &ScatterplotPlugin::updateData);
    
    qApp->installEventFilter(this);
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

    emit currentDatasetChanged(_currentDataSet);

    const Points& points = _core->requestData<Points>(_currentDataSet);

    // For source data determine whether to use dimension names or make them up
    if (points.getDimensionNames().size() == points.getNumDimensions())
        _scatterPlotSettings->initDimOptions(points.getDimensionNames());
    else
        _scatterPlotSettings->initDimOptions(points.getNumDimensions());

    // For derived data determine whether to use dimension names or make them up
    if (DataSet::getSourceData(points).getDimensionNames().size() == DataSet::getSourceData(points).getNumDimensions())
        _scatterPlotSettings->initScalarDimOptions(DataSet::getSourceData(points).getDimensionNames());
    else
        _scatterPlotSettings->initScalarDimOptions(DataSet::getSourceData(points).getNumDimensions());

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
    int xDim = _scatterPlotSettings->getXDimension();
    int yDim = _scatterPlotSettings->getYDimension();

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
    points.extractDataForDimensions(_points, _scatterPlotSettings->getXDimension(), _scatterPlotSettings->getYDimension());
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
            // Find the common global indices selected and its local index in the subset indices
            std::vector<bool> selectedPoints(sourceSet.getNumRawPoints(), false);
            for (const unsigned int& selectionIndex : selection.indices)
                selectedPoints[selectionIndex] = true;
            
            // Translate from derived data indices to subset indices
            std::vector<unsigned int> sourceIndices(_numPoints);
            for (int i = 0; i < _numPoints; i++)
            {
                const unsigned int& derivedIndex = points.indices[i];
                sourceIndices[i] = sourceSet.indices[derivedIndex];
            }
            
            for (int i = 0; i < _numPoints; i++)
            {
                if (selectedPoints[sourceIndices[i]])
                    highlights[i] = 1;
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
            // Find the common global indices selected and its local index in the subset indices
            std::vector<bool> selectedPoints(points.getNumRawPoints(), false);
            for (const unsigned int& selectionIndex : selection.indices)
                selectedPoints[selectionIndex] = true;

            for (int i = 0; i < _numPoints; i++)
            {
                if (selectedPoints[points.indices[i]])
                    highlights[i] = 1;
            }
        }
    }

    _scatterPlotWidget->setHighlights(highlights);

    emit selectionChanged();
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
        if ((_scatterPlotSettings != nullptr) && _scatterPlotSettings->IsNotifyingOnSelecting())
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

SelectionTool& ScatterplotPlugin::getSelectionTool()
{
    return *_selectionTool;
}

bool ScatterplotPlugin::eventFilter(QObject* target, QEvent* event)
{
    auto widgetBeneathCursor = QApplication::widgetAt(QCursor::pos());

    if (!isAncestorOf(widgetBeneathCursor))
        return QWidget::eventFilter(target, event);

    switch (event->type())
    {
        case QEvent::KeyPress:
        {
            auto keyEvent = static_cast<QKeyEvent *>(event);

            switch (keyEvent->key())
            {
                case Qt::Key::Key_R:
                {
                    _selectionTool->setType(SelectionTool::Type::Rectangle);
                    break;
                }

                case Qt::Key::Key_C:
                {
                    _selectionTool->setType(SelectionTool::Type::Circle);
                    break;
                }

                case Qt::Key::Key_B:
                {
                    _selectionTool->setType(SelectionTool::Type::Brush);
                    break;
                }

                case Qt::Key::Key_P:
                {
                    _selectionTool->setType(SelectionTool::Type::Polygon);
                    break;
                }

                case Qt::Key::Key_L:
                {
                    _selectionTool->setType(SelectionTool::Type::Lasso);
                    break;
                }

                case Qt::Key::Key_A:
                {
                    selectAll();
                    break;
                }

                case Qt::Key::Key_D:
                {
                    clearSelection();
                    break;
                }

                case Qt::Key::Key_I:
                {
                    invertSelection();
                    break;
                }

                case Qt::Key::Key_Shift:
                {
                    _selectionTool->setModifier(SelectionTool::Modifier::Add);
                    break;
                }

                case Qt::Key::Key_Control:
                {
                    _selectionTool->setModifier(SelectionTool::Modifier::Remove);
                    break;
                }

                case Qt::Key::Key_Escape:
                {
                    switch (_selectionTool->getType())
                    {
                        case SelectionTool::Type::Rectangle:
                        case SelectionTool::Type::Brush:
                            break;

                        case SelectionTool::Type::Lasso:
                        case SelectionTool::Type::Polygon:
                        {
                            _selectionTool->finish();
                            break;
                        }

                        default:
                            break;
                    }

                    break;
                }

                default:
                    break;
            }

            break;
        }

        case QEvent::KeyRelease:
        {
            auto keyEvent = static_cast<QKeyEvent*>(event);

            switch (keyEvent->key())
            {
                case Qt::Key::Key_R:
                case Qt::Key::Key_B:
                case Qt::Key::Key_L:
                case Qt::Key::Key_P:
                case Qt::Key::Key_A:
                case Qt::Key::Key_D:
                case Qt::Key::Key_I:
                case Qt::Key::Key_Z:
                    break;

                case Qt::Key::Key_Shift:
                case Qt::Key::Key_Control:
                {
                    _selectionTool->setModifier(SelectionTool::Modifier::Replace);
                    break;
                }

                default:
                    break;
            }

            break;
        }
    }

    return QWidget::eventFilter(target, event);
}

QString ScatterplotPlugin::getCurrentDataset() const
{
    return _currentDataSet;
}

std::uint32_t ScatterplotPlugin::getNumPoints() const
{
    const Points& points = _core->requestData<Points>(_currentDataSet);

    return points.getNumPoints();
}

std::uint32_t ScatterplotPlugin::getNumSelectedPoints() const
{
    const Points& points    = _core->requestData<Points>(_currentDataSet);
    const Points& selection = static_cast<Points&>(points.getSelection());

    return selection.indices.size();
}

void ScatterplotPlugin::selectAll()
{
    if (_currentDataSet.isEmpty())
        return;

    const Points& points = _core->requestData<Points>(_currentDataSet);
    Points& selectionSet = dynamic_cast<Points&>(points.getSelection());

    selectionSet.indices.clear();
    selectionSet.indices.resize(points.getNumPoints());

    std::iota(selectionSet.indices.begin(), selectionSet.indices.end(), 0);

    _core->notifySelectionChanged(selectionSet.getDataName());
}

void ScatterplotPlugin::clearSelection()
{
    if (_currentDataSet.isEmpty())
        return;

    const Points& points = _core->requestData<Points>(_currentDataSet);
    Points& selectionSet = dynamic_cast<Points&>(points.getSelection());

    selectionSet.indices.clear();

    _core->notifySelectionChanged(selectionSet.getDataName());
}

void ScatterplotPlugin::invertSelection()
{
    if (_currentDataSet.isEmpty())
        return;

    const Points& points = _core->requestData<Points>(_currentDataSet);
    Points& selectionSet = dynamic_cast<Points&>(points.getSelection());

    auto& indices = selectionSet.indices;

    const auto indicesSet = QSet<std::uint32_t>(indices.begin(), indices.end());

    indices.clear();
    indices.reserve(points.getNumPoints());

    for (int p = 0; p < points.getNumPoints(); ++p) {
        if (indicesSet.contains(p))
            continue;

        indices.push_back(p);
    }

    _core->notifySelectionChanged(selectionSet.getDataName());
}

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* ScatterplotPluginFactory::produce()
{
    return new ScatterplotPlugin();
}
