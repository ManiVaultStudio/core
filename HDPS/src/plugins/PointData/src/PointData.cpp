#ifndef HDPS_ASSUME_FLOAT32_POINT_DATA
// Avoid warnings about calls to deprecated PointData member functions.
#define HDPS_ASSUME_FLOAT32_POINT_DATA
#endif

#include "PointData.h"
#include "InfoAction.h"

#include <QtCore>
#include <QtDebug>
#include <QPainter>

#include <cstring>
#include <type_traits>
#include <queue>
#include <set>

#include "graphics/Vector2f.h"
#include "Application.h"

Q_PLUGIN_METADATA(IID "nl.tudelft.PointData")

// =============================================================================
// PointData
// =============================================================================

PointData::~PointData(void)
{
    
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
    _infoAction()
{
}

Points::~Points()
{
}

void Points::init()
{
    _infoAction = QSharedPointer<InfoAction>::create(nullptr, _core, *this);

    addAction(*_infoAction.get());
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

void Points::getLocalSelectionIndices(std::vector<unsigned int>& localSelectionIndices) const
{
    Points& selection = static_cast<Points&>(getSelection());

    // Find the global indices of this dataset
    std::vector<unsigned int> localGlobalIndices;
    getGlobalIndices(localGlobalIndices);

    // In an array the size of the full raw data, mark selected points as true
    std::vector<bool> globalSelection(Points::getSourceData(*this).getNumRawPoints(), false);
    for (const unsigned int& selectionIndex : selection.indices)
        globalSelection[selectionIndex] = true;

    // For all local points find out which are selected
    std::vector<bool> selected(localGlobalIndices.size(), false);
    int indexCount = 0;
    for (int i = 0; i < localGlobalIndices.size(); i++)
    {
        if (globalSelection[localGlobalIndices[i]])
        {
            selected[i] = true;
            indexCount++;
        }
    }

    localSelectionIndices.resize(indexCount);
    int c = 0;
    for (int i = 0; i < selected.size(); i++)
    {
        if (selected[i])
            localSelectionIndices[c++] = i;
    }
}

hdps::DataSet* Points::copy() const
{
    Points* set = new Points(_core, getDataName());

    set->setGuiName(getGuiName());
    set->indices = indices;

    return set;
}

DataSet& Points::createSubset(const QString subsetGuiName, DataSet* parentDataSet, const bool& visible /*= true*/) const
{
    return _core->createSubsetFromSelection(getSelection(), *this, subsetGuiName, parentDataSet, visible);
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

void Points::selectAll()
{
    auto& selection         = dynamic_cast<Points&>(getSelection());
    auto& selectionIndices  = selection.indices;

    selectionIndices.clear();
    selectionIndices.resize(getNumPoints());

    if (isFull()) {
        std::iota(selectionIndices.begin(), selectionIndices.end(), 0);
    }
    else {
        for (const auto& index : indices)
            selectionIndices.push_back(index);
    }

    _core->notifyDataSelectionChanged(*this);
}

void Points::selectNone()
{
    auto& selection         = dynamic_cast<Points&>(getSelection());
    auto& selectionIndices  = selection.indices;

    selectionIndices.clear();

    _core->notifyDataSelectionChanged(*this);
}

void Points::selectInvert()
{
    auto& selection         = dynamic_cast<Points&>(getSelection());
    auto& selectionIndices  = selection.indices;

    std::set<std::uint32_t> selectionSet(selectionIndices.begin(), selectionIndices.end());

    const auto noPixels = getNumPoints();

    selectionIndices.clear();
    selectionIndices.reserve(noPixels - selectionSet.size());

    for (std::uint32_t i = 0; i < noPixels; i++) {
        if (selectionSet.find(i) == selectionSet.end())
            selectionIndices.push_back(i);
    }

    _core->notifyDataSelectionChanged(*this);
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

void Points::addLinkedSelection(DataSet& targetDataSet, hdps::SelectionMap& mapping)
{
    _linkedSelections.emplace_back(this, &targetDataSet);
    _linkedSelections.back().setMapping(mapping);
}

void Points::setSelection(std::vector<unsigned int>& indices)
{
    Points& selection = static_cast<Points&>(getSelection());

    selection.indices = indices;

    for (hdps::LinkedSelection& linkedSelection : _linkedSelections)
    {
        const hdps::SelectionMap& mapping = linkedSelection.getMapping();

        // Create separate vector of additional linked selected points
        std::vector<unsigned int> extraSelectionIndices;
        // Reserve at least as much space as required for a 1-1 mapping
        extraSelectionIndices.reserve(indices.size());

        for (const int selectionIndex : indices)
        {
            if (mapping.find(selectionIndex) != mapping.end())
            {
                const std::vector<unsigned int>& mappedSelection = mapping.at(selectionIndex);
                extraSelectionIndices.insert(extraSelectionIndices.end(), mappedSelection.begin(), mappedSelection.end());
            }
        }

        selection.indices.insert(selection.indices.end(), extraSelectionIndices.begin(), extraSelectionIndices.end());
    }
}

// =============================================================================
// Factory
// =============================================================================

QIcon PointDataFactory::getIcon() const
{
    return Application::getIconFont("FontAwesome").getIcon("circle");
}

hdps::plugin::RawData* PointDataFactory::produce()
{
    return new PointData(this);
}
