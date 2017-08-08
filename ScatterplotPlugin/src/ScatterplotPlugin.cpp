#include "ScatterplotPlugin.h"

#include "PointsPlugin.h"

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
    widget = new hdps::gui::ScatterplotWidget();
    widget->setPointSize(10);
    widget->setAlpha(0.5f);
    widget->addSelectionListener(this);

    settings = new ScatterplotSettings();
    
    connect(&settings->_dataOptions, SIGNAL(currentIndexChanged(QString)), SLOT(dataSetPicked(QString)));
    connect(&settings->_pointSizeSlider, SIGNAL(valueChanged(int)), SLOT(pointSizeChanged(int)));
    connect(&settings->_subsetButton, SIGNAL(clicked()), SLOT(subsetCreated()));

    connect(&settings->_xDimOptions, SIGNAL(currentIndexChanged(int)), SLOT(xDimPicked(int)));
    connect(&settings->_yDimOptions, SIGNAL(currentIndexChanged(int)), SLOT(yDimPicked(int)));

    addWidget(widget);
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

    int nDim = points->numDimensions;

    settings->initDimOptions(nDim);

    updateData();
}

void ScatterplotPlugin::pointSizeChanged(const int size)
{
    widget->setPointSize(size);
}

void ScatterplotPlugin::subsetCreated()
{
    qDebug() << "Creating subset";
    const hdps::Set* set = _core->requestData(settings->currentData());
    const hdps::Set* selection = _core->requestSelection(set->getDataName());
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


void ScatterplotPlugin::updateData()
{
    qDebug() << "UPDATING";
    const IndexSet* dataSet = dynamic_cast<const IndexSet*>(_core->requestData(settings->currentData()));
    const PointsPlugin* points = dynamic_cast<const PointsPlugin*>(_core->requestPlugin(dataSet->getDataName()));
    const IndexSet* selection = dynamic_cast<const IndexSet*>(_core->requestSelection(points->getName()));
    
    std::vector<float>* positions = new std::vector<float>();
    std::vector<float> colors;

    int nDim = points->numDimensions;

    int xIndex = settings->getXDimension();
    int yIndex = settings->getYDimension();
    qDebug() << "X: " << xIndex << " Y: " << yIndex;
    if (xIndex < 0 || yIndex < 0)
        return;

    // Calculate data bounds
    float maxLength = getMaxLength(&points->data, nDim);

    if (dataSet->isFull()) {
        for (int i = 0; i < points->data.size() / nDim; i++)
        {
            positions->push_back(points->data[i * nDim + xIndex] / maxLength);
            positions->push_back(points->data[i * nDim + yIndex] / maxLength);
            if (nDim >= 5) {
                colors.push_back(points->data[i * nDim + 2]);
                colors.push_back(points->data[i * nDim + 3]);
                colors.push_back(points->data[i * nDim + 4]);
            }
        }
        for (unsigned int index : selection->indices)
        {
            colors[index * 3 + 0] = 1.0f;
            colors[index * 3 + 1] = 0.5f;
            colors[index * 3 + 2] = 1.0f;
        }
    }
    else {
        for (unsigned int index : dataSet->indices) {
            positions->push_back(points->data[index * nDim + xIndex] / maxLength);
            positions->push_back(points->data[index * nDim + yIndex] / maxLength);

            bool selected = false;
            for (unsigned int selectionIndex : selection->indices) {
                if (index == selectionIndex) {
                    selected = true;
                }
            }
            if (selected) {
                colors.push_back(1.0f);
                colors.push_back(0.5f);
                colors.push_back(1.0f);
            }
            else {
                if (nDim >= 5) {
                    colors.push_back(points->data[index * nDim + 2]);
                    colors.push_back(points->data[index * nDim + 3]);
                    colors.push_back(points->data[index * nDim + 4]);
                }
            }
        }
    }
    widget->setData(positions);
    if (nDim >= 5) {
        widget->setColors(colors);
    }qDebug() << "DONE UPDATING";
}

void ScatterplotPlugin::onSelection(const std::vector<unsigned int> selection) const
{
    if (settings->numDataOptions() == 0)
        return;

    const IndexSet* set = dynamic_cast<IndexSet*>(_core->requestData(settings->currentData()));
    IndexSet* selectionSet = dynamic_cast<IndexSet*>(_core->requestSelection(set->getDataName()));

    selectionSet->indices.clear();
    if (set->isFull())
    {
        for (unsigned int index : selection) {
            selectionSet->indices.push_back(index);
        }
    }
    else
    {
        for (unsigned int index : selection) {
            selectionSet->indices.push_back(set->indices[index]);
        }
    }

    _core->notifySelectionChanged(selectionSet->getDataName());
}

float ScatterplotPlugin::getMaxLength(const std::vector<float>* data, const int nDim) const
{
    int xIndex = settings->getXDimension();
    int yIndex = settings->getYDimension();

    float maxLength = 0;
    for (int i = 0; i < data->size() / nDim; i++) {
        float x = data->at(i * nDim + xIndex);
        float y = data->at(i * nDim + yIndex);
        float length = x*x + y*y;

        if (length > maxLength)
            maxLength = length;
    }
    return sqrt(maxLength);
}

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* ScatterplotPluginFactory::produce()
{
    return new ScatterplotPlugin();
}
