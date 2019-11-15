#include "ScatterplotSettings.h"

#include "ScatterplotPlugin.h"

#include <QGridLayout>
#include <QPainter>
#include <QStringList>

#include <cassert>

PointSettingsWidget::PointSettingsWidget(const ScatterplotPlugin& plugin) :
    _pointSizeLabel("Point Size:"),
    _pointOpacityLabel("Point Opacity:"),
    _pointSizeSlider(Qt::Horizontal),
    _pointOpacitySlider(Qt::Horizontal)
{
    connect(&_pointSizeSlider, &QSlider::valueChanged, plugin._scatterPlotWidget, &ScatterplotWidget::pointSizeChanged);
    connect(&_pointOpacitySlider, &QSlider::valueChanged, plugin._scatterPlotWidget, &ScatterplotWidget::pointOpacityChanged);

    _pointSizeSlider.setRange(MIN_POINT_SIZE, MAX_POINT_SIZE);
    _pointSizeSlider.setValue(10);

    _pointOpacitySlider.setRange(MIN_POINT_OPACITY, MAX_POINT_OPACITY);
    _pointOpacitySlider.setValue(50);

    QVBoxLayout* pointSettingsLayout = new QVBoxLayout();
    pointSettingsLayout->setMargin(0);

    HorizontalDivider* dataDivider = new HorizontalDivider();
    dataDivider->setTitle("DATA");

    pointSettingsLayout->addLayout(dataDivider);

    pointSettingsLayout->addWidget(&_pointSizeLabel);
    pointSettingsLayout->addWidget(&_pointSizeSlider);
    pointSettingsLayout->addWidget(&_pointOpacityLabel);
    pointSettingsLayout->addWidget(&_pointOpacitySlider);

    pointSettingsLayout->addStretch(1);
    setLayout(pointSettingsLayout);
}

DensitySettingsWidget::DensitySettingsWidget(const ScatterplotPlugin& plugin) :
    _sigmaLabel("Sigma:"),
    _sigmaSlider(Qt::Horizontal)
{
    connect(&_sigmaSlider, &QSlider::valueChanged, plugin._scatterPlotWidget, &ScatterplotWidget::sigmaChanged);

    _sigmaSlider.setRange(MIN_SIGMA, MAX_SIGMA);
    _sigmaSlider.setValue(30);

    QVBoxLayout* densitySettingsLayout = new QVBoxLayout();
    densitySettingsLayout->addWidget(&_sigmaLabel);
    densitySettingsLayout->addWidget(&_sigmaSlider);
    setLayout(densitySettingsLayout);
}

PlotSettingsStack::PlotSettingsStack(const ScatterplotPlugin& plugin) :
    _pointSettingsWidget(plugin),
    _densitySettingsWidget(plugin)
{
    addWidget(&_pointSettingsWidget);
    addWidget(&_densitySettingsWidget);
}

DimensionPicker::DimensionPicker(const ScatterplotPlugin* plugin) :
    _xDimLabel("X:"),
    _yDimLabel("Y:"),
    _cDimLabel("Color:"),
    _loadColorData(QStringList({ "Points", "Color", "Cluster" }))
{
    QLabel* dropLabel = new QLabel();
    dropLabel->setFixedSize(50, 50);
    //dropLabel->setStyleSheet("background-color: #FFFFFF;");
    dropLabel->setPixmap(QPixmap(":/icons/DragDropWhite.png").scaled(50, 50));
    _loadColorData.addWidget(dropLabel);

    connect(&_loadColorData, &hdps::gui::DataSlot::onDataInput, plugin, &ScatterplotPlugin::onColorDataInput);

    _layout.addWidget(&_xDimLabel, 0, 0);
    _layout.addWidget(&_yDimLabel, 1, 0);
    _layout.addWidget(&_cDimLabel, 2, 0);
    _layout.addWidget(&_loadColorData, 3, 0);

    _layout.addWidget(&_xDimOptions, 0, 1);
    _layout.addWidget(&_yDimOptions, 1, 1);
    _layout.addWidget(&_cDimOptions, 2, 1);

    connect(&_xDimOptions, SIGNAL(currentIndexChanged(int)), plugin, SLOT(xDimPicked(int)));
    connect(&_yDimOptions, SIGNAL(currentIndexChanged(int)), plugin, SLOT(yDimPicked(int)));
    connect(&_cDimOptions, SIGNAL(currentIndexChanged(int)), plugin, SLOT(cDimPicked(int)));
}

QGridLayout& DimensionPicker::getLayout()
{
    return _layout;
}

void DimensionPicker::setDimensions(unsigned int numDimensions, const std::vector<QString>& names)
{
    bool hasNames = numDimensions == names.size();

    std::vector<QComboBox*> allBoxes = { &_xDimOptions, &_yDimOptions };

    for (auto* dimensionBox : allBoxes)
    {
        dimensionBox->blockSignals(true);
        dimensionBox->clear();

        for (unsigned int i = 0; i < numDimensions; i++)
        {
            QString name = hasNames ? names[i] : "Dim " + QString::number(i);
            dimensionBox->addItem(name);
        }
    }

    if (numDimensions >= 2)
    {
        _xDimOptions.setCurrentIndex(0);
        _yDimOptions.setCurrentIndex(1);
    }

    for (auto* dimensionBox : allBoxes)
    {
        dimensionBox->blockSignals(false);
    }
}

void DimensionPicker::setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names)
{
    bool hasNames = numDimensions == names.size();

    _cDimOptions.blockSignals(true);
    _cDimOptions.clear();

    for (unsigned int i = 0; i < numDimensions; i++)
    {
        QString name = hasNames ? names[i] : "Dim " + QString::number(i);
        _cDimOptions.addItem(name);
    }

    _cDimOptions.blockSignals(false);
}

int DimensionPicker::getDimensionX()
{
    return _xDimOptions.currentIndex();
}

int DimensionPicker::getDimensionY()
{
    return _yDimOptions.currentIndex();
}

int DimensionPicker::getDimensionColor()
{
    return _cDimOptions.currentIndex();
}

ScatterplotSettings::ScatterplotSettings(const ScatterplotPlugin* plugin)
:
    _baseColor(DEFAULT_BASE_COLOR),
    _selectionColor(DEFAULT_SELECTION_COLOR)
{
    setFixedWidth(200);
    setStyleSheet(
        "QWidget { background-color:#62656c; color: white; font-family: \"Open Sans\"; font-size: 11px; }"
        //        "QFrame { background-color: #888b93; }"
    );

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

int ScatterplotSettings::getColorDimension()
{
    return _dimensionPicker->getDimensionColor();
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
