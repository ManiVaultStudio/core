#include "ScatterplotPlugin.h"

#include "ToolBar.h"
#include "PointData.h"
#include "ClusterData.h"
#include "ColorData.h"
#include "Application.h"

#include "graphics/Vector2f.h"
#include "graphics/Vector3f.h"

#include <QtCore>
#include <QApplication>
#include <QToolBar>
#include <QDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.ScatterplotPlugin")

using namespace hdps;

ScatterplotPlugin::ScatterplotPlugin() :
    ViewPlugin("Scatterplot View"),
    _currentDataSet(),
    _supportedColorTypes(),
    _dataSlot(nullptr),
    _points(),
    _numPoints(0),
    _pixelSelectionTool(new PixelSelectionTool(this, false)),
    _scatterPlotWidget(new ScatterplotWidget(*_pixelSelectionTool)),
    _settingsWidget(new SettingsWidget(*this))
{
    setDockingLocation(DockableWidget::DockingLocation::Right);
}

// =============================================================================
// View
// =============================================================================

ScatterplotPlugin::~ScatterplotPlugin(void)
{
    
}

void ScatterplotPlugin::init()
{
    _dataSlot = new DataSlot(supportedDataTypes());
    _supportedColorTypes.append(PointType);
    _supportedColorTypes.append(ClusterType);
    _supportedColorTypes.append(ColorType);

    _scatterPlotWidget->setAlpha(0.5f);

    _scatterPlotWidget->setRenderMode(ScatterplotWidget::RenderMode::SCATTERPLOT);
    _dataSlot->addWidget(_scatterPlotWidget);

    _dataSlot->layout()->setMargin(0);

    auto layout = new QVBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(_settingsWidget);
    layout->addWidget(_dataSlot, 1);

    setLayout(layout);

    connect(_dataSlot, &DataSlot::onDataInput, this, &ScatterplotPlugin::onDataInput);
    connect(_scatterPlotWidget, &ScatterplotWidget::initialized, this, &ScatterplotPlugin::updateData);
    
    qApp->installEventFilter(this);

    QObject::connect(_pixelSelectionTool, &PixelSelectionTool::areaChanged, [this]() {
        if (!_pixelSelectionTool->isNotifyDuringSelection())
            return;

        selectPoints();
    });

    QObject::connect(_pixelSelectionTool, &PixelSelectionTool::ended, [this]() {
        if (_pixelSelectionTool->isNotifyDuringSelection())
            return;

        selectPoints();
    });
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

void ScatterplotPlugin::selectPoints()
{
    if (_currentDataSet.isEmpty() || !_pixelSelectionTool->isActive())
        return;

    auto selectionAreaImage = _pixelSelectionTool->getAreaPixmap().toImage();

    const Points& points = _core->requestData<Points>(_currentDataSet);
    Points& selectionSet = dynamic_cast<Points&>(points.getSelection());

    std::vector<std::uint32_t> targetIndices;

    targetIndices.reserve(points.getNumPoints());

    const auto dataBounds   = _scatterPlotWidget->getBounds();
    const auto width        = selectionAreaImage.width();
    const auto height       = selectionAreaImage.height();
    const auto size         = width < height ? width : height;
    const auto& set         = points.isDerivedData() ? DataSet::getSourceData(points) : points;
    const auto& setIndices  = set.indices;

    for (unsigned int i = 0; i < _points.size(); i++) {
        const auto uvNormalized     = QPointF((_points[i].x - dataBounds.getLeft()) / dataBounds.getWidth(), (dataBounds.getTop() - _points[i].y) / dataBounds.getHeight());
        const auto uvOffset         = QPoint((selectionAreaImage.width() - size) / 2.0f, (selectionAreaImage.height() - size) / 2.0f);
        const auto uv               = uvOffset + QPoint(uvNormalized.x() * size, uvNormalized.y() * size);

        if (selectionAreaImage.pixelColor(uv).alpha() > 0) {
            if (set.isFull())
                targetIndices.push_back(i);
            else
                targetIndices.push_back(setIndices[i]);
        }
    }
    
    auto& selectionSetIndices = selectionSet.indices;

    switch (_pixelSelectionTool->getModifier())
    {
        case PixelSelectionTool::Modifier::Replace:
            break;

        case PixelSelectionTool::Modifier::Add:
        case PixelSelectionTool::Modifier::Remove:
        {
            QSet<std::uint32_t> set(selectionSetIndices.begin(), selectionSetIndices.end());

            switch (_pixelSelectionTool->getModifier())
            {
                case PixelSelectionTool::Modifier::Add:
                {
                    for (const auto& targetIndex : targetIndices)
                        set.insert(targetIndex);

                    break;
                }

                case PixelSelectionTool::Modifier::Remove:
                {
                    for (const auto& targetIndex : targetIndices)
                        set.remove(targetIndex);

                    break;
                }

                default:
                    break;
            }

            targetIndices = std::vector<std::uint32_t>(set.begin(), set.end());

            break;
        }

        default:
            break;
    }

    selectionSetIndices = targetIndices;

    _core->notifySelectionChanged(points.isDerivedData() ? DataSet::getSourceData(points).getDataName() : points.getDataName());
}

void ScatterplotPlugin::onDataInput(QString dataSetName)
{
    _currentDataSet = dataSetName;

    setWindowTitle(_currentDataSet);

    emit currentDatasetChanged(_currentDataSet);

    const Points& points = _core->requestData<Points>(_currentDataSet);

    // For source data determine whether to use dimension names or make them up
    if (points.getDimensionNames().size() == points.getNumDimensions())
        _settingsWidget->initDimOptions(points.getDimensionNames());
    else
        _settingsWidget->initDimOptions(points.getNumDimensions());

    // For derived data determine whether to use dimension names or make them up
    if (DataSet::getSourceData(points).getDimensionNames().size() == DataSet::getSourceData(points).getNumDimensions())
        _settingsWidget->initScalarDimOptions(DataSet::getSourceData(points).getDimensionNames());
    else
        _settingsWidget->initScalarDimOptions(DataSet::getSourceData(points).getNumDimensions());

    updateData();

    _pixelSelectionTool->setEnabled(!_currentDataSet.isEmpty());
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
    int xDim = _settingsWidget->getXDimension();
    int yDim = _settingsWidget->getYDimension();

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
    points.extractDataForDimensions(_points, _settingsWidget->getXDimension(), _settingsWidget->getYDimension());
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

PixelSelectionTool& ScatterplotPlugin::getSelectionTool()
{
    return *_pixelSelectionTool;
}

bool ScatterplotPlugin::eventFilter(QObject* target, QEvent* event)
{
    auto widgetBeneathCursor = QApplication::widgetAt(QCursor::pos());

    if (!isAncestorOf(widgetBeneathCursor) || _currentDataSet.isEmpty())
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
                    _pixelSelectionTool->setType(PixelSelectionTool::Type::Rectangle);
                    break;
                }

                case Qt::Key::Key_B:
                {
                    _pixelSelectionTool->setType(PixelSelectionTool::Type::Brush);
                    break;
                }

                case Qt::Key::Key_P:
                {
                    _pixelSelectionTool->setType(PixelSelectionTool::Type::Polygon);
                    break;
                }

                case Qt::Key::Key_L:
                {
                    _pixelSelectionTool->setType(PixelSelectionTool::Type::Lasso);
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
                    _pixelSelectionTool->setModifier(PixelSelectionTool::Modifier::Add);
                    break;
                }

                case Qt::Key::Key_Control:
                {
                    _pixelSelectionTool->setModifier(PixelSelectionTool::Modifier::Remove);
                    break;
                }

                case Qt::Key::Key_Escape:
                {
                    switch (_pixelSelectionTool->getType())
                    {
                        case PixelSelectionTool::Type::Rectangle:
                        case PixelSelectionTool::Type::Brush:
                            break;

                        case PixelSelectionTool::Type::Lasso:
                        case PixelSelectionTool::Type::Polygon:
                            _pixelSelectionTool->abort();
                            break;

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
                case Qt::Key::Key_Shift:
                case Qt::Key::Key_Control:
                {
                    _pixelSelectionTool->setModifier(PixelSelectionTool::Modifier::Replace);
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
    if (_currentDataSet.isEmpty())
        return 0;

    const Points& points = _core->requestData<Points>(_currentDataSet);

    return points.getNumPoints();
}

std::uint32_t ScatterplotPlugin::getNumSelectedPoints() const
{
    const Points& points    = _core->requestData<Points>(_currentDataSet);
    const Points& selection = static_cast<Points&>(points.getSelection());

    return selection.indices.size();
}

bool ScatterplotPlugin::canSelect() const
{
    return !getCurrentDataset().isEmpty() && getNumPoints() >= 0;
}

bool ScatterplotPlugin::canSelectAll() const
{
    return getNumPoints() == -1 ? false : getNumSelectedPoints() != getNumPoints();
}

bool ScatterplotPlugin::canClearSelection() const
{
    return getNumPoints() == -1 ? false : getNumSelectedPoints() >= 1;
}

bool ScatterplotPlugin::canInvertSelection() const
{
    return getNumPoints() >= 0;
}

void ScatterplotPlugin::selectAll()
{
    if (_currentDataSet.isEmpty())
        return;

    const auto& points = _core->requestData<Points>(_currentDataSet);

    auto& selectionSet          = dynamic_cast<Points&>(points.getSelection());
    auto& selectionSetIndices   = selectionSet.indices;

    const auto& setIndices = points.isDerivedData() ? DataSet::getSourceData(points).indices : points.indices;

    selectionSetIndices.clear();
    selectionSetIndices.resize(points.getNumPoints());

    if (points.isFull())
        std::iota(selectionSetIndices.begin(), selectionSetIndices.end(), 0);
    else
        selectionSetIndices = setIndices;

    _core->notifySelectionChanged(selectionSet.getDataName());
}

void ScatterplotPlugin::clearSelection()
{
    if (_currentDataSet.isEmpty())
        return;

    const auto& points = _core->requestData<Points>(_currentDataSet);

    auto& selectionSet          = dynamic_cast<Points&>(points.getSelection());
    auto& selectionSetIndices   = selectionSet.indices;

    selectionSetIndices.clear();

    _core->notifySelectionChanged(selectionSet.getDataName());
}

void ScatterplotPlugin::invertSelection()
{
    if (_currentDataSet.isEmpty())
        return;

    const auto& points = _core->requestData<Points>(_currentDataSet);

    auto& selectionSet          = dynamic_cast<Points&>(points.getSelection());
    auto& selectionSetIndices   = selectionSet.indices;

    const auto& pointsIndices = points.isDerivedData() ? DataSet::getSourceData(points).indices : points.indices;

    const auto selectionIndicesSet = QSet<std::uint32_t>(selectionSetIndices.begin(), selectionSetIndices.end());

    selectionSetIndices.clear();
    selectionSetIndices.reserve(points.getNumPoints());

    for (int p = 0; p < points.getNumPoints(); ++p) {
        const auto setIndex = points.isFull() ? p : pointsIndices[p];

        if (selectionIndicesSet.contains(setIndex))
            continue;

        selectionSetIndices.push_back(setIndex);
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
