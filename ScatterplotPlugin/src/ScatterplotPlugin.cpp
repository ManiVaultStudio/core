#include "ScatterplotPlugin.h"

#include "PointsPlugin.h"
#include "SelectionPlugin.h"

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
    
    connect(&dataOptions, SIGNAL(currentIndexChanged(QString)), SLOT(dataSetPicked(QString)));
    connect(&pointSizeSlider, SIGNAL(valueChanged(int)), SLOT(pointSizeChanged(int)));
    addWidget(&dataOptions);
    addWidget(widget);
    addWidget(&pointSizeSlider);
}

void ScatterplotPlugin::dataAdded(const QString name)
{
    DataTypePlugin* data = _core->requestData(name);
    if (data->getKind() == "Points" || data->getKind() == "Selection") {
        dataOptions.addItem(name);
    }
}

void ScatterplotPlugin::dataChanged(const QString name)
{
    if (name != dataOptions.currentText()) {
        return;
    }
    updateData(name);
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
    updateData(name);
}

void ScatterplotPlugin::pointSizeChanged(const int size)
{
    widget->setPointSize(size);
}


void ScatterplotPlugin::updateData(const QString name)
{
    DataTypePlugin* data = _core->requestData(name);
    
    std::vector<float>* positions = new std::vector<float>();
    std::vector<float> colors;

    if (data->getKind() == "Points")
    {
        const PointsPlugin* points = dynamic_cast<const PointsPlugin*>(data);

        for (int i = 0; i < points->data.size() / 5; i++)
        {
            positions->push_back(points->data[i * 5 + 0]);
            positions->push_back(points->data[i * 5 + 1]);
            colors.push_back(points->data[i * 5 + 2]);
            colors.push_back(points->data[i * 5 + 3]);
            colors.push_back(points->data[i * 5 + 4]);
        }
    }
    else if (data->getKind() == "Selection")
    {
        const SelectionPlugin* selection = dynamic_cast<const SelectionPlugin*>(data);

        const PointsPlugin* points = dynamic_cast<const PointsPlugin*>(_core->requestData(selection->parentName));

        for (unsigned int index : selection->indices)
        {
            positions->push_back(points->data[index * 5 + 0]);
            positions->push_back(points->data[index * 5 + 1]);
            colors.push_back(points->data[index * 5 + 2]);
            colors.push_back(points->data[index * 5 + 3]);
            colors.push_back(points->data[index * 5 + 4]);
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
    QString name = _core->addData("Selection");

    DataTypePlugin* selectionData = _core->requestData(name);
    SelectionPlugin* selectionPlugin = dynamic_cast<SelectionPlugin*>(selectionData);

    selectionPlugin->parentName = dataOptions.currentText();
    for (unsigned int index : selection) {
        selectionPlugin->indices.push_back(index);
    }

    _core->notifyDataChanged(name);
}

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* ScatterplotPluginFactory::produce()
{
    return new ScatterplotPlugin();
}
