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
    const PointsSet* dataSet = dynamic_cast<const PointsSet*>(_core->requestData(name));
    const DataTypePlugin* data = _core->requestPlugin(dataSet->getDataName());
    const PointsSet* selection = dynamic_cast<const PointsSet*>(_core->requestSelection(data->getName()));
    
    std::vector<float>* positions = new std::vector<float>();
    std::vector<float> colors;

    if (data->getKind() == "Points")
    {
        const PointsPlugin* points = dynamic_cast<const PointsPlugin*>(data);

        if (dataSet->isFull()) {
            for (int i = 0; i < points->data.size() / 5; i++)
            {
                positions->push_back(points->data[i * 5 + 0]);
                positions->push_back(points->data[i * 5 + 1]);
                colors.push_back(points->data[i * 5 + 2]);
                colors.push_back(points->data[i * 5 + 3]);
                colors.push_back(points->data[i * 5 + 4]);
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
                positions->push_back(points->data[index * 5 + 0]);
                positions->push_back(points->data[index * 5 + 1]);
                colors.push_back(points->data[index * 5 + 2]);
                colors.push_back(points->data[index * 5 + 3]);
                colors.push_back(points->data[index * 5 + 4]);
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
    const hdps::Set* set = _core->requestData(dataOptions.currentText());
    PointsSet* selectionSet = dynamic_cast<PointsSet*>(_core->requestSelection(set->getDataName()));

    selectionSet->indices.clear();
    for (unsigned int index : selection) {
        selectionSet->indices.push_back(index);
    }

    _core->notifyDataChanged(dataOptions.currentText());
}

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* ScatterplotPluginFactory::produce()
{
    return new ScatterplotPlugin();
}
