#include "SettingsWidget.h"
#include "PointSettingsWidget.h"
#include "DensitySettingsWidget.h"
#include "RenderModeWidget.h"
#include "PlotSettingsWidget.h"
#include "DimensionPickerWidget.h"
#include "SelectionToolWidget.h"

#include "ScatterplotPlugin.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QPainter>

#include <cassert>

ScatterplotSettings::ScatterplotSettings(const ScatterplotPlugin* plugin)
:
    _baseColor(DEFAULT_BASE_COLOR),
    _selectionColor(DEFAULT_SELECTION_COLOR)
{
    setMinimumWidth(250);

    _subsetButton.setText("Create Subset");
    _subsetButton.setFixedWidth(100);

    _settingsLayout = new QVBoxLayout();

    QVBoxLayout* dataLayout = new QVBoxLayout();
    dataLayout->addWidget(&_subsetButton);
    
    QVBoxLayout* renderLayout = new QVBoxLayout();
    _renderMode.addItem("Scatterplot");
    _renderMode.addItem("Density map");
    _renderMode.addItem("Contour map");
    _renderMode.setFixedWidth(100);
    renderLayout->addWidget(&_renderMode);

    _plotSettingsWidget = new PlotSettingsWidget(*plugin);
    renderLayout->addWidget(_plotSettingsWidget);

    _renderModeWidget = new RenderModeWidget(*plugin);
    _dimensionPickerWidget = new DimensionPickerWidget(*plugin);

    _settingsLayout->addLayout(dataLayout);
    _settingsLayout->addLayout(renderLayout, 1);
    _settingsLayout->addWidget(_renderModeWidget);
    _settingsLayout->addWidget(_dimensionPickerWidget);
    _settingsLayout->addWidget(new SelectionToolWidget(const_cast<ScatterplotPlugin*>(plugin)));

    setLayout(_settingsLayout);

    connect(&_subsetButton, SIGNAL(clicked()), plugin, SLOT(subsetCreated()));

    connect(&_renderMode, qOverload<int>(&QComboBox::currentIndexChanged), [this, plugin](int index) {
        plugin->_scatterPlotWidget->setRenderMode(static_cast<ScatterplotWidget::RenderMode>(index));
    });
}

ScatterplotSettings::~ScatterplotSettings()
{

}

int ScatterplotSettings::getXDimension()
{
    return _dimensionPickerWidget->getDimensionX();
}

int ScatterplotSettings::getYDimension()
{
    return _dimensionPickerWidget->getDimensionY();
}

hdps::Vector3f ScatterplotSettings::getBaseColor()
{
    return _baseColor;
}

hdps::Vector3f ScatterplotSettings::getSelectionColor()
{
    return _selectionColor;
}


void ScatterplotSettings::initDimOptions(const unsigned int nDim)
{
    _dimensionPickerWidget->setDimensions(nDim);
}

void ScatterplotSettings::initDimOptions(const std::vector<QString>& dimNames)
{
    _dimensionPickerWidget->setDimensions(dimNames.size(), dimNames);
}

void ScatterplotSettings::initScalarDimOptions(const unsigned int nDim)
{
    _dimensionPickerWidget->setScalarDimensions(nDim);
}

void ScatterplotSettings::initScalarDimOptions(const std::vector<QString>& dimNames)
{
    _dimensionPickerWidget->setScalarDimensions(dimNames.size(), dimNames);
}

void ScatterplotSettings::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}