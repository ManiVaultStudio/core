#include "ScatterplotSettings.h"

#include "ScatterplotPlugin.h"

#include <QGridLayout>
#include <QLabel>

ScatterplotSettings::ScatterplotSettings(const ScatterplotPlugin* plugin)
:
    _baseColor(DEFAULT_BASE_COLOR),
    _selectionColor(DEFAULT_SELECTION_COLOR)
{
    setFixedHeight(100);

    _subsetButton.setText("Create Subset");
    _subsetButton.setFixedWidth(100);

    _settingsLayout = new QGridLayout();
    _settingsLayout->setColumnStretch(2, 1);
    _settingsLayout->addWidget(&_dataOptions, 0, 0);
    
    _renderMode.addItem("Scatterplot");
    _renderMode.addItem("Density map");
    _renderMode.addItem("Contour map");
    _settingsLayout->addWidget(&_renderMode, 1, 0);

    _settingsLayout->addWidget(&_subsetButton, 0, 1);

    _pointSettingsWidget._pointSizeSlider.setRange(MIN_POINT_SIZE, MAX_POINT_SIZE);
    _densitySettingsWidget._sigmaSlider.setRange(MIN_SIGMA, MAX_SIGMA);


    _settingsStack = new QStackedWidget();
    _settingsStack->addWidget(&_pointSettingsWidget);
    _settingsStack->addWidget(&_densitySettingsWidget);
    _settingsLayout->addWidget(_settingsStack, 1, 1, 1, 2);

    QLabel* xDimLabel = new QLabel("X:");
    QLabel* yDimLabel = new QLabel("Y:");

    _settingsLayout->addWidget(xDimLabel, 0, 3);
    _settingsLayout->addWidget(&_xDimOptions, 0, 4);
    _settingsLayout->addWidget(yDimLabel, 1, 3);
    _settingsLayout->addWidget(&_yDimOptions, 1, 4);

    setLayout(_settingsLayout);

    connect(&_dataOptions, SIGNAL(currentIndexChanged(QString)), plugin, SLOT(dataSetPicked(QString)));
    connect(&_subsetButton, SIGNAL(clicked()), plugin, SLOT(subsetCreated()));
    connect(&_xDimOptions, SIGNAL(currentIndexChanged(int)), plugin, SLOT(xDimPicked(int)));
    connect(&_yDimOptions, SIGNAL(currentIndexChanged(int)), plugin, SLOT(yDimPicked(int)));

    connect(&_renderMode, SIGNAL(currentIndexChanged(int)), plugin->_scatterPlotWidget, SLOT(renderModePicked(int)));
    connect(&_renderMode, SIGNAL(currentIndexChanged(int)), this, SLOT(renderModePicked(int)));
    connect(&_densitySettingsWidget._sigmaSlider, SIGNAL(valueChanged(int)), plugin->_scatterPlotWidget, SLOT(sigmaChanged(int)));
    connect(&_pointSettingsWidget._pointSizeSlider, SIGNAL(valueChanged(int)), plugin->_scatterPlotWidget, SLOT(pointSizeChanged(int)));

    _pointSettingsWidget._pointSizeSlider.setValue(10);
    _densitySettingsWidget._sigmaSlider.setValue(30);
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

hdps::Vector3f ScatterplotSettings::getBaseColor()
{
    return _baseColor;
}

hdps::Vector3f ScatterplotSettings::getSelectionColor()
{
    return _selectionColor;
}

void ScatterplotSettings::showPointSettings()
{
    _settingsStack->setCurrentIndex(0);
}

void ScatterplotSettings::showDensitySettings()
{
    _settingsStack->setCurrentIndex(1);
}

QString ScatterplotSettings::currentData()
{
    return _dataOptions.currentText();
}

void ScatterplotSettings::initDimOptions(const unsigned int nDim)
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

void ScatterplotSettings::addDataOption(const QString option)
{
    _dataOptions.addItem(option);
}

int ScatterplotSettings::numDataOptions()
{
    return _dataOptions.count();
}

void ScatterplotSettings::renderModePicked(const int index)
{
    switch (index)
    {
    case 0: showPointSettings(); break;
    case 1: showDensitySettings(); break;
    case 2: showDensitySettings(); break;
    }
}
