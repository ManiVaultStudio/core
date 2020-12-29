#include "SettingsWidget.h"
#include "PointSettingsWidget.h"
#include "SelectionToolWidget.h"

#include "ScatterplotPlugin.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QPainter>
#include <QStringListModel>

#include <cassert>

namespace
{
    QStringListModel& createStringListModel(const unsigned int numDimensions, const std::vector<QString>& names, QObject& parent)
    {
        QStringList stringList;

        stringList.reserve(static_cast<int>(numDimensions));

        if (numDimensions == names.size())
        {
            for (const auto& name : names)
            {
                stringList.append(name);
            }
        }
        else
        {
            for (unsigned int i = 0; i < numDimensions; ++i)
            {
                stringList.append(QString::fromLatin1("Dim %1").arg(i));
            }
        }

        auto* stringListModel = new QStringListModel(stringList, &parent);
        return *stringListModel;
    }
}

DensitySettingsWidget::DensitySettingsWidget(const ScatterplotPlugin& plugin) :
    _sigmaLabel(*new QLabel("Sigma:")),
    _sigmaSlider(*new QSlider(Qt::Horizontal)),
    _computeDensityButton(*new QPushButton("Compute density"))
{
    connect(&_sigmaSlider, &QSlider::valueChanged, plugin._scatterPlotWidget, &ScatterplotWidget::sigmaChanged);
    connect(&_computeDensityButton, &QPushButton::pressed, plugin._scatterPlotWidget, &ScatterplotWidget::computeDensity);

    _sigmaSlider.setRange(MIN_SIGMA, MAX_SIGMA);
    _sigmaSlider.setValue(30);

    QVBoxLayout* densitySettingsLayout = new QVBoxLayout();
    densitySettingsLayout->addWidget(&_sigmaLabel);
    densitySettingsLayout->addWidget(&_sigmaSlider);
    densitySettingsLayout->addWidget(&_computeDensityButton);

    densitySettingsLayout->addStretch(1);
    setLayout(densitySettingsLayout);
}

PlotSettingsStack::PlotSettingsStack(const ScatterplotPlugin& plugin) :
    _densitySettingsWidget(*new DensitySettingsWidget(plugin))
{
    addWidget(new PointSettingsWidget(plugin));
    addWidget(&_densitySettingsWidget);
}

ColorDimensionPicker::ColorDimensionPicker(const ScatterplotPlugin& plugin)
{
    _layout = new QVBoxLayout();
    _cDimOptions = new QComboBox();
    _cDimOptions->setFixedWidth(100);
    _layout->addWidget(_cDimOptions);
    setLayout(_layout);

    connect(_cDimOptions, SIGNAL(currentIndexChanged(int)), &plugin, SLOT(cDimPicked(int)));
}

void ColorDimensionPicker::setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names)
{
    auto& stringListModel = createStringListModel(numDimensions, names, *this);

    _cDimOptions->blockSignals(true);
    _cDimOptions->setModel(&stringListModel);
    _cDimOptions->blockSignals(false);
}

ColorDropSlot::ColorDropSlot(const ScatterplotPlugin& plugin) :
    _loadColorData(*new hdps::gui::DataSlot(plugin.supportedColorTypes))
{
    QLabel* dropLabel = new QLabel();
    dropLabel->setFixedSize(20, 20);
    dropLabel->setPixmap(QPixmap(":/icons/DragDropWhite.png").scaled(20, 20));
    QLabel* dropBox = new QLabel();
    dropBox->setFixedSize(100, 30);
    dropBox->setStyleSheet("QLabel { background-color: white; color: #696969; border: none; border-radius: 2px; font-family: \"Open Sans\",Helvetica,Arial,sans-serif; font-size: 12px; }");
    
    _loadColorData.addWidget(dropBox);

    connect(&_loadColorData, &hdps::gui::DataSlot::onDataInput, &plugin, &ScatterplotPlugin::onColorDataInput);

    _layout = new QHBoxLayout();
    _layout->addWidget(dropLabel);
    _layout->addWidget(&_loadColorData);
    setLayout(_layout);
}

DimensionPicker::DimensionPicker(const ScatterplotPlugin* plugin) :
    _xDimLabel(*new QLabel( "X:")),
    _yDimLabel(*new QLabel("Y:")),
    _cDimLabel(*new QLabel("Color:"))
{
    _layout.addWidget(&_xDimLabel, 0, 0);
    _layout.addWidget(&_yDimLabel, 1, 0);
    _layout.addWidget(&_cDimLabel, 2, 0);

    _colorOptions = new QComboBox();
    _colorOptions->addItem("Color By Dimension");
    _colorOptions->addItem("Color By Data");
    _layout.addWidget(&_xDimOptions, 0, 1);
    _layout.addWidget(&_yDimOptions, 1, 1);
    _layout.addWidget(_colorOptions, 2, 1);
    _colorSettingsStack = new ColorSettingsStack(*plugin);
    _layout.addWidget(_colorSettingsStack, 3, 1);

    connect(&_xDimOptions, SIGNAL(currentIndexChanged(int)), plugin, SLOT(xDimPicked(int)));
    connect(&_yDimOptions, SIGNAL(currentIndexChanged(int)), plugin, SLOT(yDimPicked(int)));
    connect(_colorOptions, SIGNAL(currentIndexChanged(int)), this, SLOT(colorOptionsPicked(int)));
}

QGridLayout& DimensionPicker::getLayout()
{
    return _layout;
}

void DimensionPicker::setDimensions(unsigned int numDimensions, const std::vector<QString>& names)
{
    auto& stringListModel = createStringListModel(numDimensions, names, *this);

    QComboBox* const allBoxes[] = { &_xDimOptions, &_yDimOptions };

    for (auto* const dimensionBox : allBoxes)
    {
        dimensionBox->blockSignals(true);
        dimensionBox->setModel(&stringListModel);
    }

    if (numDimensions >= 2)
    {
        _xDimOptions.setCurrentIndex(0);
        _yDimOptions.setCurrentIndex(1);
    }

    for (auto* const dimensionBox : allBoxes)
    {
        dimensionBox->blockSignals(false);
    }
}

void DimensionPicker::setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names)
{
    _colorSettingsStack->getColorDimensionPicker().setScalarDimensions(numDimensions, names);
}

int DimensionPicker::getDimensionX()
{
    return _xDimOptions.currentIndex();
}

int DimensionPicker::getDimensionY()
{
    return _yDimOptions.currentIndex();
}

void DimensionPicker::colorOptionsPicked(const int index)
{
    switch (index)
    {
    case 0: _colorSettingsStack->setCurrentIndex(0); break;
    case 1: _colorSettingsStack->setCurrentIndex(1); break;
    }
}

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

    _settingsStack = new PlotSettingsStack(*plugin);
    renderLayout->addWidget(_settingsStack);

    _dimensionPicker = new DimensionPicker(plugin);

    _settingsLayout->addLayout(dataLayout);
    _settingsLayout->addLayout(renderLayout, 1);
    _settingsLayout->addLayout(&_dimensionPicker->getLayout());
    _settingsLayout->addWidget(new SelectionToolWidget(const_cast<ScatterplotPlugin*>(plugin)));

    setLayout(_settingsLayout);

    connect(&_subsetButton, SIGNAL(clicked()), plugin, SLOT(subsetCreated()));

    connect(&_renderMode, SIGNAL(currentIndexChanged(int)), plugin->_scatterPlotWidget, SLOT(renderModePicked(int)));
    connect(&_renderMode, SIGNAL(currentIndexChanged(int)), this, SLOT(renderModePicked(int)));
}

ScatterplotSettings::~ScatterplotSettings()
{

}

int ScatterplotSettings::getXDimension()
{
    return _dimensionPicker->getDimensionX();
}

int ScatterplotSettings::getYDimension()
{
    return _dimensionPicker->getDimensionY();
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

void ScatterplotSettings::initDimOptions(const unsigned int nDim)
{
    _dimensionPicker->setDimensions(nDim);
}

void ScatterplotSettings::initDimOptions(const std::vector<QString>& dimNames)
{
    _dimensionPicker->setDimensions(dimNames.size(), dimNames);
}

void ScatterplotSettings::initScalarDimOptions(const unsigned int nDim)
{
    _dimensionPicker->setScalarDimensions(nDim);
}

void ScatterplotSettings::initScalarDimOptions(const std::vector<QString>& dimNames)
{
    _dimensionPicker->setScalarDimensions(dimNames.size(), dimNames);
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

void ScatterplotSettings::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}