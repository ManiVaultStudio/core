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

    subsetButton.setText("Create Subset");
    
    connect(&dataOptions, SIGNAL(currentIndexChanged(QString)), SLOT(dataSetPicked(QString)));
    connect(&pointSizeSlider, SIGNAL(valueChanged(int)), SLOT(pointSizeChanged(int)));
    connect(&subsetButton, SIGNAL(clicked()), SLOT(subsetCreated()));

    connect(&xDimOptions, SIGNAL(currentIndexChanged(int)), SLOT(xDimPicked(int)));
    connect(&yDimOptions, SIGNAL(currentIndexChanged(int)), SLOT(yDimPicked(int)));

    addWidget(&dataOptions);
    addWidget(widget);
    addWidget(&pointSizeSlider);
    addWidget(&subsetButton);
    addWidget(&xDimOptions);
    addWidget(&yDimOptions);
}

void ScatterplotPlugin::dataAdded(const QString name)
{
    dataOptions.addItem(name);
}

void ScatterplotPlugin::dataChanged(const QString name)
{
    if (name != dataOptions.currentText()) {
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
    const IndexSet* dataSet = dynamic_cast<const IndexSet*>(_core->requestData(dataOptions.currentText()));
    const PointsPlugin* points = dynamic_cast<const PointsPlugin*>(_core->requestPlugin(dataSet->getDataName()));

    int nDim = points->numDimensions;

    xDimOptions.clear();
    yDimOptions.clear();
    for (int i = 0; i < nDim; i++)
    {
        xDimOptions.addItem(QString::number(i));
        yDimOptions.addItem(QString::number(i));
    }

    if (nDim >= 2)
    {
        xDimOptions.setCurrentIndex(0);
        yDimOptions.setCurrentIndex(1);
    }

    updateData();
}

void ScatterplotPlugin::pointSizeChanged(const int size)
{
    widget->setPointSize(size);
}

void ScatterplotPlugin::subsetCreated()
{
    qDebug() << "Creating subset";
    const hdps::Set* set = _core->requestData(dataOptions.currentText());
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
    const IndexSet* dataSet = dynamic_cast<const IndexSet*>(_core->requestData(dataOptions.currentText()));
    const PointsPlugin* points = dynamic_cast<const PointsPlugin*>(_core->requestPlugin(dataSet->getDataName()));
    const IndexSet* selection = dynamic_cast<const IndexSet*>(_core->requestSelection(points->getName()));
    
    std::vector<float>* positions = new std::vector<float>();
    std::vector<float> colors;

    int nDim = points->numDimensions;

    int xIndex = xDimOptions.currentIndex();
    int yIndex = yDimOptions.currentIndex();
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
    if (dataOptions.count() == 0)
        return;

    const IndexSet* set = dynamic_cast<IndexSet*>(_core->requestData(dataOptions.currentText()));
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
    int xIndex = xDimOptions.currentIndex();
    int yIndex = yDimOptions.currentIndex();

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
