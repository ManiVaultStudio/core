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
    addWidget(&dataOptions);
    addWidget(widget);
    addWidget(&pointSizeSlider);
    addWidget(&subsetButton);
}

void ScatterplotPlugin::dataAdded(const QString name)
{
    const hdps::Set* set = _core->requestData(name);
    const DataTypePlugin* dataPlugin = _core->requestPlugin(set->getDataName());

    if (dataPlugin->getKind() == "Points") {
        dataOptions.addItem(name);
    }
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


void ScatterplotPlugin::updateData()
{
    const PointsSet* dataSet = dynamic_cast<const PointsSet*>(_core->requestData(dataOptions.currentText()));
    const DataTypePlugin* data = _core->requestPlugin(dataSet->getDataName());
    const PointsSet* selection = dynamic_cast<const PointsSet*>(_core->requestSelection(data->getName()));
    
    std::vector<float>* positions = new std::vector<float>();
    std::vector<float> colors;

    if (data->getKind() == "Points")
    {
        const PointsPlugin* points = dynamic_cast<const PointsPlugin*>(data);
        int nDim = points->numDimensions;

        if (dataSet->isFull()) {
            for (int i = 0; i < points->data.size() / nDim; i++)
            {
                positions->push_back(points->data[i * nDim + 0]);
                positions->push_back(points->data[i * nDim + 1]);
                colors.push_back(points->data[i * nDim + 2]);
                colors.push_back(points->data[i * nDim + 3]);
                colors.push_back(points->data[i * nDim + 4]);
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
                positions->push_back(points->data[index * nDim + 0]);
                positions->push_back(points->data[index * nDim + 1]);

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
                    colors.push_back(points->data[index * nDim + 2]);
                    colors.push_back(points->data[index * nDim + 3]);
                    colors.push_back(points->data[index * nDim + 4]);
                }
            }
        }
    }
    else
    {
        qWarning((QString("Scatterplot plugin doesn't support data of kind: ") + data->getKind()).toStdString().c_str());
        return;
    }

    widget->setData(positions);
    widget->setColors(colors);
}

void ScatterplotPlugin::onSelection(const std::vector<unsigned int> selection) const
{
    if (dataOptions.count() == 0)
        return;

    const PointsSet* set = dynamic_cast<PointsSet*>(_core->requestData(dataOptions.currentText()));
    PointsSet* selectionSet = dynamic_cast<PointsSet*>(_core->requestSelection(set->getDataName()));

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

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* ScatterplotPluginFactory::produce()
{
    return new ScatterplotPlugin();
}
