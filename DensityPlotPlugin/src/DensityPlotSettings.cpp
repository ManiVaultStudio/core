#include "DensityPlotSettings.h"

#include "DensityPlotPlugin.h"

#include <QGridLayout>
#include <QLabel>

DensityPlotSettings::DensityPlotSettings(const DensityPlotPlugin* plugin)
{
    setFixedHeight(100);

    _subsetButton.setText("Create Subset");
    _subsetButton.setFixedWidth(100);

    _renderMode.addItem("Density");
    _renderMode.addItem("Gradient");
    _renderMode.addItem("Partitions");
    _renderMode.addItem("Landscape");

    QGridLayout *settingsLayout = new QGridLayout();
    settingsLayout->setColumnStretch(2, 1);
    settingsLayout->addWidget(&_dataOptions, 0, 0);

    settingsLayout->addWidget(&_subsetButton, 1, 0);

    settingsLayout->addWidget(&_renderMode, 0, 1);

    QLabel* xDimLabel = new QLabel("X:");
    QLabel* yDimLabel = new QLabel("Y:");

    settingsLayout->addWidget(xDimLabel, 0, 3);
    settingsLayout->addWidget(&_xDimOptions, 0, 4);
    settingsLayout->addWidget(yDimLabel, 1, 3);
    settingsLayout->addWidget(&_yDimOptions, 1, 4);

    setLayout(settingsLayout);

    connect(&_dataOptions, SIGNAL(currentIndexChanged(QString)), plugin, SLOT(dataSetPicked(QString)));
    connect(&_subsetButton, SIGNAL(clicked()), plugin, SLOT(subsetCreated()));

    connect(&_xDimOptions, SIGNAL(currentIndexChanged(int)), plugin, SLOT(xDimPicked(int)));
    connect(&_yDimOptions, SIGNAL(currentIndexChanged(int)), plugin, SLOT(yDimPicked(int)));

    connect(&_renderMode, SIGNAL(currentIndexChanged(int)), plugin, SLOT(renderModePicked(int)));
}

DensityPlotSettings::~DensityPlotSettings()
{

}

int DensityPlotSettings::getXDimension()
{
    return _xDimOptions.currentIndex();
}

int DensityPlotSettings::getYDimension()
{
    return _yDimOptions.currentIndex();
}

QString DensityPlotSettings::currentData()
{
    return _dataOptions.currentText();
}

void DensityPlotSettings::initDimOptions(const unsigned int nDim)
{
    _xDimOptions.blockSignals(true);
    _yDimOptions.blockSignals(true);

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

    _xDimOptions.blockSignals(false);
    _yDimOptions.blockSignals(false);
}

void DensityPlotSettings::addDataOption(const QString option)
{
    _dataOptions.addItem(option);
}

int DensityPlotSettings::numDataOptions()
{
    return _dataOptions.count();
}
