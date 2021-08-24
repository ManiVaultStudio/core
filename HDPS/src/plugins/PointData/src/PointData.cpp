#ifndef HDPS_ASSUME_FLOAT32_POINT_DATA
// Avoid warnings about calls to deprecated PointData member functions.
#define HDPS_ASSUME_FLOAT32_POINT_DATA
#endif

#include "PointData.h"
#include "InfoAction.h"
#include "ClusterAction.h"

#include <QtCore>
#include <QtDebug>
#include <QPainter>

#include <cstring>
#include <type_traits>
#include <queue>

#include "graphics/Vector2f.h"
#include "Application.h"

Q_PLUGIN_METADATA(IID "nl.tudelft.PointData")

// =============================================================================
// PointData
// =============================================================================

PointData::~PointData(void)
{
    
}

QIcon PointData::getIcon() const
{
    return hdps::Application::getIconFont("FontAwesome").getIcon("circle");
}

void PointData::init()
{

}

hdps::DataSet* PointData::createDataSet() const
{
    return new Points(_core, getName());
}

unsigned int PointData::getNumPoints() const
{
    return static_cast<unsigned int>(_vectorHolder.size() / _numDimensions);
}

unsigned int PointData::getNumDimensions() const
{
    return _numDimensions;
}

const std::vector<QString>& PointData::getDimensionNames() const
{
    return _dimNames;
}

void PointData::setData(const std::nullptr_t, const std::size_t numPoints, const std::size_t numDimensions)
{
    _vectorHolder.resize(numPoints * numDimensions);
    _numDimensions = static_cast<unsigned int>(numDimensions);
}

void PointData::setDimensionNames(const std::vector<QString>& dimNames)
{
    _dimNames = dimNames;
}

float PointData::getValueAt(const std::size_t index) const
{
    return _vectorHolder.constVisit<float>([index](const auto& vec)
        {
            return vec[index];
        });
}

void PointData::setValueAt(const std::size_t index, const float newValue)
{
    _vectorHolder.visit([index, newValue](auto& vec)
        {
            using value_type = typename std::remove_reference_t<decltype(vec)>::value_type;
            vec[index] = static_cast<value_type>(newValue);
        });
}

void PointData::extractFullDataForDimension(std::vector<float>& result, const int dimensionIndex) const
{
    CheckDimensionIndex(dimensionIndex);

    result.resize(getNumPoints());

    _vectorHolder.constVisit(
        [&result, this, dimensionIndex](const auto& vec)
        {
            const auto resultSize = result.size();

            for (std::size_t i{}; i < resultSize; ++i)
            {
                result[i] = vec[i * _numDimensions + dimensionIndex];
            }
        });
}


void PointData::extractFullDataForDimensions(std::vector<hdps::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2) const
{
    CheckDimensionIndex(dimensionIndex1);
    CheckDimensionIndex(dimensionIndex2);

    result.resize(getNumPoints());

    _vectorHolder.constVisit(
        [&result, this, dimensionIndex1, dimensionIndex2](const auto& vec)
        {
            const auto resultSize = result.size();

            for (std::size_t i{}; i < resultSize; ++i)
            {
                const auto n = i * _numDimensions;
                result[i].set(vec[n + dimensionIndex1], vec[n + dimensionIndex2]);
            }
        });
}


void PointData::extractDataForDimensions(std::vector<hdps::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2, const std::vector<unsigned int>& indices) const
{
    CheckDimensionIndex(dimensionIndex1);
    CheckDimensionIndex(dimensionIndex2);

    result.resize(indices.size());

    _vectorHolder.constVisit(
        [&result, this, dimensionIndex1, dimensionIndex2, indices](const auto& vec)
        {
            const auto resultSize = result.size();

            for (std::size_t i{}; i < resultSize; ++i)
            {
                const auto n = std::size_t{ indices[i] } *_numDimensions;
                result[i].set(vec[n + dimensionIndex1], vec[n + dimensionIndex2]);
            }
        });
}

Points::Points(hdps::CoreInterface* core, QString dataName) :
    hdps::DataSet(core, dataName),
    _infoAction(),
    _clusterAction()
{
}

Points::~Points()
{
}

void Points::init()
{
    _infoAction     = QSharedPointer<InfoAction>::create(nullptr, _core, getName());
    _clusterAction  = QSharedPointer<ClusterAction>::create(nullptr, _core, getName());

    addAction(*_infoAction.get());
    addAction(*_clusterAction.get());
}

// =============================================================================
// Point Set
// =============================================================================


void Points::setData(std::nullptr_t, const std::size_t numPoints, const std::size_t numDimensions)
{
    getRawData<PointData>().setData(nullptr, numPoints, numDimensions);
}

void Points::extractDataForDimension(std::vector<float>& result, const int dimensionIndex) const
{
    // This overload assumes that the data set is "full".
    // Please remove the assert once non-full support is implemented (if necessary).
    assert(isFull());

    const auto& rawPointData = getRawData<PointData>();
    rawPointData.extractFullDataForDimension(result, dimensionIndex);
}


void Points::extractDataForDimensions(std::vector<hdps::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2) const
{
    const auto& rawPointData = getRawData<PointData>();

    if (isFull())
    {
        rawPointData.extractFullDataForDimensions(result, dimensionIndex1, dimensionIndex2);
    }
    else
    {
        rawPointData.extractDataForDimensions(result, dimensionIndex1, dimensionIndex2, indices);
    }
}

void Points::getGlobalIndices(std::vector<unsigned int>& globalIndices) const
{
    const Points* currentDataset = this;

    std::queue<const Points*> subsetChain;
    // Walk back in the chain of derived data until we find the original source
    while (currentDataset->isDerivedData())
    {
        // If the current set is a subset then store it on the stack to traverse later
        if (!isFull())
        {
            subsetChain.push(currentDataset);
        }
        currentDataset = &Points::getSourceData(*currentDataset);
    }

    // We now have a non-derived dataset bound, push it if its also a subset
    if (!currentDataset->isFull())
        subsetChain.push(currentDataset);

    // Traverse down the stack applying indexing
    globalIndices.resize(getNumPoints(), 0);
    std::iota(globalIndices.begin(), globalIndices.end(), 0);

    while (!subsetChain.empty())
    {
        const Points& subset = *subsetChain.front();
        subsetChain.pop();

        for (int i = 0; i < globalIndices.size(); i++)
        {
            globalIndices[i] = subset.indices[globalIndices[i]];
        }
    }
}

void Points::selectedLocalIndices(const std::vector<unsigned int>& selectionIndices, std::vector<bool>& selected) const
{
    // Find the global indices of this dataset
    std::vector<unsigned int> localGlobalIndices;
    getGlobalIndices(localGlobalIndices);

    // In an array the size of the full raw data, mark selected points as true
    std::vector<bool> globalSelection(Points::getSourceData(*this).getNumRawPoints(), false);
    for (const unsigned int& selectionIndex : selectionIndices)
        globalSelection[selectionIndex] = true;

    // For all local points find out which are selected
    selected.resize(localGlobalIndices.size(), false);
    for (int i = 0; i < localGlobalIndices.size(); i++)
    {
        if (globalSelection[localGlobalIndices[i]])
            selected[i] = true;
    }
}

hdps::DataSet* Points::copy() const
{
    Points* set = new Points(_core, getDataName());
    set->setName(getName());
    set->indices = indices;
    return set;
}

QString Points::createSubset(const QString parentSetName /*= ""*/, const bool& visible /*= true*/) const
{
    const hdps::DataSet& selection = getSelection();

    return _core->createSubsetFromSelection(selection, *this, "Subset", parentSetName, visible);
}

QIcon Points::getIcon() const
{
    return hdps::Application::getIconFont("FontAwesome").getIcon("database");

    /*
    const auto size = QSize(100, 100);

    QPixmap pixmap(size);

    pixmap.fill(Qt::transparent);

    const auto iconRectangle = QRect(0, 0, size.width(), size.height());

    QPainter painter(&pixmap);

    painter.setRenderHint(QPainter::Antialiasing);

    QPen linepen(Qt::black);
    linepen.setCapStyle(Qt::RoundCap);
    linepen.setWidth(15);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(linepen);

    QVector<QPoint> points;

    QPolygon polygon;

    const auto numSteps     = 3;
    const auto stepSizeX    = static_cast<float>(size.width()) / static_cast<float>(numSteps + 1);
    const auto stepSizeY    = static_cast<float>(size.height()) / static_cast<float>(numSteps + 1);

    for (int x = stepSizeX; x < size.width(); x += stepSizeX)
        for (int y = stepSizeY; y < size.height(); y += stepSizeY)
            polygon << QPoint(x, y);

    painter.drawPoints(polygon);

    return QIcon(pixmap);
    */
}

const std::vector<QString>& Points::getDimensionNames() const
{
    return getRawData<PointData>().getDimensionNames();
}

void Points::setDimensionNames(const std::vector<QString>& dimNames)
{
    getRawData<PointData>().setDimensionNames(dimNames);
}

float Points::getValueAt(const std::size_t index) const
{
    return getRawData<PointData>().getValueAt(index);
}

void Points::setValueAt(const std::size_t index, const float newValue)
{
    getRawData<PointData>().setValueAt(index, newValue);
}

// =============================================================================
// Factory
// =============================================================================

hdps::plugin::RawData* PointDataFactory::produce()
{
    return new PointData(this);
}
