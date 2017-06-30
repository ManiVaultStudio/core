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
    
    connect(&dataOptions, SIGNAL(currentIndexChanged(QString)), SLOT(dataSetPicked(QString)));
    connect(&pointSizeSlider, SIGNAL(valueChanged(int)), SLOT(pointSizeChanged(int)));
    addWidget(&dataOptions);
    addWidget(widget);
    addWidget(&pointSizeSlider);
}

void ScatterplotPlugin::dataAdded(const QString name)
{
    DataTypePlugin* data = _core->requestPlugin(name);
    if (data->getKind() == "Points") {
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
    const PointsSet* set = dynamic_cast<const PointsSet*>(_core->requestData(name));
    DataTypePlugin* data = _core->requestPlugin(set->getDataName());
    const PointsSet* selection = dynamic_cast<const PointsSet*>(_core->requestSelection(name));
    
    std::vector<float>* positions = new std::vector<float>();
    std::vector<float> colors;

    if (data->getKind() == "Points")
    {
        const PointsPlugin* points = dynamic_cast<const PointsPlugin*>(data);

        if (set->isFull()) {
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
            for (unsigned int index : set->indices) {
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
    PointsSet* selectionSet = dynamic_cast<PointsSet*>(_core->requestSelection(dataOptions.currentText()));

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
