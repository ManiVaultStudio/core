#include "ScatterplotSettings.h"

#include "ScatterplotPlugin.h"

#include <QGridLayout>
#include <QLabel>

ScatterplotSettings::ScatterplotSettings(const ScatterplotPlugin* plugin)
:
    _pointSizeSlider(Qt::Horizontal)
{
    setFixedHeight(100);

    _subsetButton.setText("Create Subset");
    _subsetButton.setFixedWidth(100);

    QGridLayout *settingsLayout = new QGridLayout();
    settingsLayout->setColumnStretch(2, 1);
    settingsLayout->addWidget(&_dataOptions, 0, 0);

    QLabel* pointSizeLabel = new QLabel("Point Size:");
    settingsLayout->addWidget(pointSizeLabel, 1, 0);
    _pointSizeSlider.setRange(1, 20);
    _pointSizeSlider.setValue(plugin->pointSize());
    settingsLayout->addWidget(&_pointSizeSlider, 1, 1, 1, 2);
    settingsLayout->addWidget(&_subsetButton, 0, 1);

    QLabel* xDimLabel = new QLabel("X:");
    QLabel* yDimLabel = new QLabel("Y:");

    settingsLayout->addWidget(xDimLabel, 0, 3);
    settingsLayout->addWidget(&_xDimOptions, 0, 4);
    settingsLayout->addWidget(yDimLabel, 1, 3);
    settingsLayout->addWidget(&_yDimOptions, 1, 4);

    setLayout(settingsLayout);

    connect(&_dataOptions, SIGNAL(currentIndexChanged(QString)), plugin, SLOT(dataSetPicked(QString)));
    connect(&_pointSizeSlider, SIGNAL(valueChanged(int)), plugin, SLOT(pointSizeChanged(int)));
    connect(&_subsetButton, SIGNAL(clicked()), plugin, SLOT(subsetCreated()));

    connect(&_xDimOptions, SIGNAL(currentIndexChanged(int)), plugin, SLOT(xDimPicked(int)));
    connect(&_yDimOptions, SIGNAL(currentIndexChanged(int)), plugin, SLOT(yDimPicked(int)));
}

ScatterplotSettings::~ScatterplotSettings()
{

}

int ScatterplotSettings::getXDimension()
{
    return _xDimOptions.currentIndex();
}

int ScatterplotSettings::getYDimension()
{
    return _yDimOptions.currentIndex();
}

QString ScatterplotSettings::currentData()
{
    return _dataOptions.currentText();
}

void ScatterplotSettings::initDimOptions(const unsigned int nDim)
{
    _xDimOptions.clear();
    _yDimOptions.clear();
    for (unsigned int i = 0; i < nDim; i++)
    {
        _xDimOptions.addItem(QString::number(i));
        _yDimOptions.addItem(QString::number(i));
    }

    if (nDim >= 2)
    {
        _xDimOptions.setCurrentIndex(0);
        _yDimOptions.setCurrentIndex(1);
    }
}

void ScatterplotSettings::addDataOption(const QString option)
{
    _dataOptions.addItem(option);
}

int ScatterplotSettings::numDataOptions()
{
    return _dataOptions.count();
}
